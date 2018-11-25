/* tc-mrisc32.c -- MRISC32 assembler
   Copyright (C) 2018 Free Software Foundation, Inc.

   Contributed by Marcus Geelnard (m@bitsnbites.eu).
   Based on the moxie and RISC-V targets.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "as.h"
#include "safe-ctype.h"
#include "tc-mrisc32.h"
#include "opcode/mrisc32.h"
#include "elf/mrisc32.h"

/* Register numbers for special register aliases.
   The PC register is only accessible for certain instructions. We put it
   in a virtual register slot (> 31) during operand decode and resolve it
   later when we know what instruction we're dealing with.  */
#define MR32_REG_Z   0
#define MR32_REG_TP  27
#define MR32_REG_FP  28
#define MR32_REG_SP  29
#define MR32_REG_LR  30
#define MR32_REG_VL  31
#define MR32_REG_VPC 100

/* This array holds the chars that always start a comment.  If the
    pre-processor is disabled, these aren't very useful */
const char comment_chars[] = ";";

/* This array holds the chars that only start a comment at the beginning of
   a line.  If the line seems to have the form '# 123 filename'
   .line and .file directives will appear in the pre-processed output */
/* Note that input_file.c hand checks for '#' at the beginning of the
   first line of the input file.  This is because the compiler outputs
   #NO_APP at the beginning of its output.  */
/* Also note that C style comments are always supported.  */
const char line_comment_chars[] = ";#";

/* This array holds machine specific line separator characters.  */
const char line_separator_chars[] = "";

/* Chars that can be used to separate mant from exp in floating point nums */
const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant */
/* As in 0f12.456 */
/* or    0d1.2345e12 */
const char FLT_CHARS[] = "rRsSfFdDxXpP";

const pseudo_typeS md_pseudo_table[] =
{
  {0, 0, 0}
};

/* Target specific command line options.  */
enum options
{
  OPTION_RELAX = OPTION_MD_BASE,
  OPTION_NO_RELAX
};

struct option md_longopts[] =
{
  {"mrelax", no_argument, NULL, OPTION_RELAX},
  {"mno-relax", no_argument, NULL, OPTION_NO_RELAX},
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof (md_longopts);

const char *md_shortopts = "";

/* Internal state.  */
static bfd_boolean s_mrisc32_relax = TRUE;
static htab_t s_opc_type_a_map;
static htab_t s_opc_type_b_map[4];
static htab_t s_opc_type_c_map;
static htab_t s_opc_type_d_map;
static htab_t s_opc_type_e_map;

#define MAX_OP_STR_LEN 10
static char s_op_str[MAX_OP_STR_LEN + 1];

enum mr32_packmode
{
  MR32_PM_NONE = 0,
  MR32_PM_BYTE = 1,
  MR32_PM_HALFWORD = 2
};

enum mr32_regscale
{
  MR32_RS_NONE = -1,
  MR32_RS_1 = 0,
  MR32_RS_2 = 1,
  MR32_RS_4 = 2,
  MR32_RS_8 = 3
};

enum mr32_bitmode
{
  MR32_BM_NONE = -1,
  MR32_BM_PP = 0,
  MR32_BM_PN = 1,
  MR32_BM_NP = 2,
  MR32_BM_NN = 3
};

enum mr32_selmode
{
  MR32_SM_NONE = -1,
  MR32_SM_123 = 0,
  MR32_SM_132 = 1,
  MR32_SM_213 = 2,
  MR32_SM_231 = 3
};

enum mr32_widthmode
{
  MR32_WM_NONE = -1,
  MR32_WM_8 = 0,
  MR32_WM_16 = 1,
  MR32_WM_32 = 2
};

enum mr32_operand_type
{
  MR32_OT_BAD = 0,
  MR32_OT_NONE,
  MR32_OT_SREG,
  MR32_OT_VREG,
  MR32_OT_EXPRESSION
};

enum mr32_modifier
{
  MR32_MOD_NONE = 0,
  MR32_MOD_HI,
  MR32_MOD_LO,
  MR32_MOD_PCHI,
  MR32_MOD_PCLO,
  MR32_MOD_PC
};

struct mr32_operand_t
{
  enum mr32_operand_type type;
  enum mr32_modifier modifier;
  enum mr32_regscale reg_scale;
  int reg_no;
  expressionS exp;
};

static void
_clear_operand (struct mr32_operand_t *operand)
{
  operand->type = MR32_OT_NONE;
  operand->modifier = MR32_MOD_NONE;
  operand->reg_scale = MR32_RS_NONE;
  operand->reg_no = -1;
  memset (&operand->exp, 0, sizeof (expressionS));
}

static bfd_boolean
_decode_suffix (char *suffix, char *suffix_end, enum mr32_packmode *pack_mode,
		enum mr32_bitmode *bit_mode, enum mr32_selmode *sel_mode,
		enum mr32_widthmode *width_mode, bfd_boolean *fold)
{
  const int len = (int) (suffix_end - suffix);
  int num_parsed_chars = 0;

  *pack_mode = MR32_PM_NONE;
  *bit_mode = MR32_BM_NONE;
  *sel_mode = MR32_SM_NONE;
  *width_mode = MR32_WM_NONE;
  *fold = FALSE;

  /* Valid suffix regex: (\.(b|h|132|213|231|pn|np|nn))?(/f)?  */

  /* The pack, bit, sel and width modes are given by the first 2 - 4
     characters of the suffix.  */
  if (len >= 2 && suffix[0] == '.')
    {
      if (len >= 4 && suffix[1] == '1' && suffix[2] == '3' && suffix[3] == '2')
	{
	  *sel_mode = MR32_SM_132;
	  num_parsed_chars += 4;
	}
      else if (len >= 4 && suffix[1] == '2' && suffix[2] == '1' &&
	       suffix[3] == '3')
	{
	  *sel_mode = MR32_SM_213;
	  num_parsed_chars += 4;
	}
      else if (len >= 4 && suffix[1] == '2' && suffix[2] == '3' &&
	       suffix[3] == '1')
	{
	  *sel_mode = MR32_SM_231;
	  num_parsed_chars += 4;
	}
      else if (len >= 3 && suffix[1] == 'p' && suffix[2] == 'n')
	{
	  *bit_mode = MR32_BM_PN;
	  num_parsed_chars += 3;
	}
      else if (len >= 3 && suffix[1] == 'n' && suffix[2] == 'p')
	{
	  *bit_mode = MR32_BM_NP;
	  num_parsed_chars += 3;
	}
      else if (len >= 3 && suffix[1] == 'n' && suffix[2] == 'n')
	{
	  *bit_mode = MR32_BM_NN;
	  num_parsed_chars += 3;
	}
      else if (len >= 3 && suffix[1] == '1' && suffix[2] == '6')
	{
	  *width_mode = MR32_WM_16;
	  num_parsed_chars += 3;
	}
      else if (len >= 3 && suffix[1] == '3' && suffix[2] == '2')
	{
	  *width_mode = MR32_WM_32;
	  num_parsed_chars += 3;
	}
      else if (suffix[1] == 'b')
	{
	  *pack_mode = MR32_PM_BYTE;
	  num_parsed_chars += 2;
	}
      else if (suffix[1] == 'h')
	{
	  *pack_mode = MR32_PM_HALFWORD;
	  num_parsed_chars += 2;
	}
      else if (suffix[1] == '8')
	{
	  *width_mode = MR32_WM_8;
	  num_parsed_chars += 2;
	}
      else
	{
	  as_bad (_ ("invalid pack, bit, sel or width mode"));
	  return FALSE;
	}
    }

  /* Folding is given as the last two characters of the suffix.  */
  char fold_char = 0;
  if (len >= 2 && suffix[len - 2] == '/')
    fold_char = suffix[len - 1];
  if (fold_char == 'f')
    {
      *fold = TRUE;
      num_parsed_chars += 2;
    }
  else if (fold_char != 0)
    {
      as_bad (_ ("invalid folding mode"));
      return FALSE;
    }

  /* Check that the suffix was fully parsed.  */
  if (num_parsed_chars != len)
    {
      as_bad (_ ("invalid op suffix"));
      return FALSE;
    }

  return TRUE;
}

static enum mr32_regscale
_get_reg_scale (const char *str, int *len)
{
  int i;
  int scale_start = -1;
  int old_len = *len;

  /* Early out if the string is too long for a register name (max 3 chars)
     plus a scale factor (max 2 chars).  */
  if (*len > 5)
    return MR32_RS_NONE;

  /* Find the first '*'.  */
  for (i = 0; i < *len; ++i)
    {
      if (str[i] == '*')
	{
	  scale_start = i + 1;
	  *len = i;
	  break;
	}
    }

  /* No '*' -> no scale.  */
  if (scale_start < 0)
    return MR32_RS_NONE;

  /* Parse the scaling factor. Valid values: "1", "2", "4", and "8".  */
  if ((old_len - scale_start) == 1)
    {
      switch (str[scale_start])
	{
	case '1':
	  return MR32_RS_1;
	case '2':
	  return MR32_RS_2;
	case '4':
	  return MR32_RS_4;
	case '8':
	  return MR32_RS_8;
	}
    }

  /* Something is amiss.  */
  char scale_str[6];
  strncpy (&scale_str[0], &str[scale_start], sizeof (scale_str) - 1);
  scale_str[sizeof (scale_str) - 1] = 0;
  as_bad (_ ("invalid register scaling factor: %s"), scale_str);
  return MR32_RS_NONE;
}

static int
_get_sreg_no (const char *name, const int len, enum mr32_regscale *reg_scale)
{
  int len_wo_scale;

  /* Parse the register scale, and drop the scale suffix.  */
  len_wo_scale = len;
  *reg_scale = _get_reg_scale (name, &len_wo_scale);

  /* Parse the register name.  */
  if (len_wo_scale == 1)
    {
      if (name[0] == 'z')
	return 0;
    }
  else if (len_wo_scale == 2)
    {
      if (name[0] == 'p' && name[1] == 'c')
	return MR32_REG_VPC;
      if (name[0] == 'v' && name[1] == 'l')
	return MR32_REG_VL;
      if (name[0] == 'l' && name[1] == 'r')
	return MR32_REG_LR;
      if (name[0] == 's' && name[1] == 'p')
	return MR32_REG_SP;
      if (name[0] == 'f' && name[1] == 'p')
	return MR32_REG_FP;
      if (name[0] == 't' && name[1] == 'p')
	return MR32_REG_TP;
      if (name[0] == 'r')
	{
	  const int reg_no = name[1] - '0';
	  if (reg_no >= 0 && reg_no <= 9)
	    return reg_no;
	}
    }
  else if (len_wo_scale == 3)
    {
      if (name[0] == 'r')
	{
	  const int reg_no_hi = name[1] - '0';
	  const int reg_no_lo = name[2] - '0';
	  if (reg_no_hi >= 1 && reg_no_hi <= 3 && reg_no_lo >= 0 &&
	      reg_no_lo <= 9)
	    {
	      const int reg_no = (10 * reg_no_hi) + reg_no_lo;
	      if (reg_no >= 0 && reg_no <= 31)
		return reg_no;
	    }
	}
    }

  return -1;
}

static int
_get_vreg_no (const char *name, const int len, enum mr32_regscale *reg_scale)
{
  int len_wo_scale;

  /* Parse the register scale, and drop the scale suffix.  */
  len_wo_scale = len;
  *reg_scale = _get_reg_scale (name, &len_wo_scale);

  /* Parse the register name.  */
  if (len_wo_scale == 2)
    {
      if (name[0] == 'v')
	{
	  if (name[1] == 'z')
	    return 0;
	  else
	    {
	      const int reg_no = name[1] - '0';
	      if (reg_no >= 0 && reg_no <= 9)
		return reg_no;
	    }
	}
    }
  else if (len_wo_scale == 3)
    {
      if (name[0] == 'v')
	{
	  const int reg_no_hi = name[1] - '0';
	  const int reg_no_lo = name[2] - '0';
	  if (reg_no_hi >= 1 && reg_no_hi <= 3 && reg_no_lo >= 0 &&
	      reg_no_lo <= 9)
	    {
	      const int reg_no = (10 * reg_no_hi) + reg_no_lo;
	      if (reg_no >= 0 && reg_no <= 31)
		return reg_no;
	    }
	}
    }

  return -1;
}

static bfd_boolean
_is_reg (const struct mr32_operand_t *operand)
{
  return operand->type == MR32_OT_SREG || operand->type == MR32_OT_VREG;
}

static bfd_boolean
_is_imm (const struct mr32_operand_t *operand)
{
  return operand->type == MR32_OT_EXPRESSION;
}

/* Check if this opcode interprets scalar register no. 31 as the
   program counter (PC) or the vector length (VL).  */
static bfd_boolean
_is_reg31_pc (const struct mrisc32_opc_info_t *opcode)
{
  /* Only opcodes with format D and mode MR32_MODE_REGREL21x4 can
     address PC as a source register.  */
  return opcode->format == MR32_FMT_D && opcode->mode == MR32_MODE_REGREL21x4;
}

static char * _decode_expression (char *str, expressionS *exp);

static char *
_try_decode_bit_field_expression (char *str, expressionS *exp)
{
  char *bf_start = str + 1;
  char *bf_mid = NULL;
  char *bf_end;

  /* Bit field expression syntax:
     <A:B> shall be decoded as ((B)<<8) | (A)  */

  if (str[0] != '<')
    return NULL;

  for (bf_end = bf_start; *bf_end != '>' && *bf_end != 0; ++bf_end)
    {
      if (*bf_end == ':')
	bf_mid = bf_end;
    }
  if (*bf_end == '>' && bf_mid != NULL && (bf_mid - bf_start) > 0 && (bf_end - bf_mid) > 1)
    {
      char *bf_o = bf_start;
      char *bf_w = bf_mid + 1;
      expressionS exp_o;
      expressionS exp_w;
      *bf_mid = 0;
      _decode_expression (bf_o, &exp_o);
      *bf_mid = ':';
      *bf_end = 0;
      _decode_expression (bf_w, &exp_w);
      *bf_end = '>';

      if (exp_o.X_op == O_constant && exp_w.X_op == O_constant)
	{
	  uint32_t o = (uint32_t) exp_o.X_add_number;
	  uint32_t w = (uint32_t) exp_w.X_add_number;
	  if (o >= 32)
	    {
	      as_bad (_ ("invalid offset"));
	      return NULL;
	    }
	  if (w >= 32)
	    w = 0;
	  *exp = exp_o;
	  exp->X_add_number = (offsetT) ((w << 8) | o);
	  return bf_end + 1;
	}
    }

  return NULL;
}

static char *
_decode_expression (char *str, expressionS *exp)
{
  char *str_end;
  char *save;

  /* Handle bit field expressions.  */
  str_end = _try_decode_bit_field_expression (str, exp);
  if (str_end != NULL)
    return str_end;

  /* Use expression() to decode the zero-terminated string STR.  */
  save = input_line_pointer;
  input_line_pointer = str;
  expression (exp);
  str_end = input_line_pointer;
  input_line_pointer = save;

  return str_end;
}

static char *
_decode_operand (char *str, struct mr32_operand_t *operand)
{
  char *str_end;
  char *mod;
  int str_len;
  int reg_no;
  enum mr32_regscale reg_scale;

  /* Clear the operand info.  */
  _clear_operand (operand);

  /* Define the string of the operand (and find the end of the operand
     string).  */
  for (str_end = str; !is_end_of_line[(unsigned char) *str_end] &&
		      !ISSPACE (*str_end) && *str_end != ',';
       ++str_end)
    ;
  str_len = (int) (str_end - str);

  /* Decode the operand type.  */
  if (str_len > 0)
    {
      if ((reg_no = _get_sreg_no (str, str_len, &reg_scale)) >= 0)
	{
	  operand->type = MR32_OT_SREG;
	  operand->reg_no = reg_no;
	  operand->reg_scale = reg_scale;
	}
      else if ((reg_no = _get_vreg_no (str, str_len, &reg_scale)) >= 0)
	{
	  operand->type = MR32_OT_VREG;
	  operand->reg_no = reg_no;
	  operand->reg_scale = reg_scale;
	}
      else
	{
	  /* Drop the optional # prefix.  */
	  if (*str == '#')
	    ++str;

	  /* Find the optional operand modifier '@'.  */
	  for (mod = str; mod < str_end && *mod != '@'; ++mod)
	    ;
	  if (mod < str_end)
	    {
	      /* Decode operand modifiers: @hi, @lo, @pchi, @pclo, @pc.  */
	      /* TODO(m): Make sure that there are no trailing chars.  */
	      if (strncmp (mod, "@hi", 3) == 0)
		operand->modifier = MR32_MOD_HI;
	      else if (strncmp (mod, "@lo", 3) == 0)
		operand->modifier = MR32_MOD_LO;
	      else if (strncmp (mod, "@pchi", 5) == 0)
		operand->modifier = MR32_MOD_PCHI;
	      else if (strncmp (mod, "@pclo", 5) == 0)
		operand->modifier = MR32_MOD_PCLO;
	      else if (strncmp (mod, "@pc", 3) == 0)
		operand->modifier = MR32_MOD_PC;

	      /* Temporarily terminate the string at the @ (so that it is not
		 included in _decode_expression).  */
	      *mod = 0;
	    }

	  /* Decode the expression.  */
	  (void) _decode_expression (str, &operand->exp);

	  /* Restore the @ in the string, if necessary.  */
	  if (mod < str_end)
	    *mod = '@';

	  if (operand->exp.X_op != O_illegal && operand->exp.X_op != O_absent)
	    {
	      operand->type = MR32_OT_EXPRESSION;
	    }
	  else
	    {
	      as_bad (_ ("invalid operand"));
	      operand->type = MR32_OT_BAD;
	    }
	}
    }

  /* Find the start of the next operand.  */
  for (; !is_end_of_line[(unsigned char) *str_end] &&
	 (ISSPACE (*str_end) || *str_end == ',');
       ++str_end)
    ;
  return str_end;
}

static char *
_decode_address_operands (char *str, struct mr32_operand_t *operand1,
			  struct mr32_operand_t *operand2)
{
  char *str_end;
  char *operands_end;

  /* Find the start and length of address specification.  */
  if (!is_end_of_line[(unsigned char) *str] && *str == '[')
    ++str;
  for (str_end = str; !is_end_of_line[(unsigned char) *str_end] &&
		      *str_end != ']'; ++str_end)
    ;
  if (*str_end != ']')
    {
      as_bad (_ ("invalid address specifier"));
      _clear_operand (operand1);
      _clear_operand (operand2);
      return str_end;
    }

  /* Temporarily terminate the string at the ending ] character.  */
  *str_end = 0;

  /* Decode up to two operands.  */
  operands_end = _decode_operand (str, operand1);
  operands_end = _decode_operand (operands_end, operand2);

  /* Check that we consumed all the operands.  */
  if (operands_end != str_end)
    as_warn (_ ("trailing address operand extras ignored: %s"), operands_end);

  /* Restore the ] character.  */
  *str_end = ']';

  /* Handle implicit operands.  */
  if (_is_reg (operand1) && operand2->type == MR32_OT_NONE)
    {
      /* Tranlaste "[reg]" to "[reg, #0]".  */
      char operand_str[3] = "#0";
      _decode_operand (&operand_str[0], operand2);
    }

  /* Find the start of the next operand.  */
  str_end++;
  for (; !is_end_of_line[(unsigned char) *str_end] &&
	 (ISSPACE (*str_end) || *str_end == ','); ++str_end)
    ;
  return str_end;
}

static bfd_boolean
_encode_reg (const struct mr32_operand_t *operand,
	     const bfd_boolean reg31_is_pc,
	     uint32_t *reg_enc)
{
  if (operand->reg_no >= 0 && operand->reg_no <= 30)
    {
      *reg_enc = (uint32_t)operand->reg_no;
      return TRUE;
    }
  else if ((operand->reg_no == MR32_REG_VPC && reg31_is_pc)
	   || (operand->reg_no == 31 && !reg31_is_pc))
    {
      *reg_enc = 31;
      return TRUE;
    }
  else
    return FALSE;
}

static bfd_boolean
_can_use_imm14lo (const uint32_t x)
{
  const int32_t sx = (int32_t) x;
  return sx >= -8192 && sx <= 8191;
}

static bfd_boolean
_can_use_imm14hi (const uint32_t x)
{
  return (x & 0x7ffffu) == 0u || (x & 0x7ffffu) == 0x7ffffu;
}

static bfd_boolean
_can_use_imm15 (const uint32_t x)
{
  const int32_t sx = (int32_t) x;
  return sx >= -16384 && sx <= 16383;
}

static bfd_boolean
_can_use_imm20lo (const uint32_t x)
{
  return ((int32_t) x) >= -524288 && ((int32_t) x) < 524288;
}

static bfd_boolean
_can_use_imm20hi (const uint32_t x)
{
  return ((x & 0x1fffu) == 0u) || ((x & 0x1fffu) == 0x1fffu);
}

static bfd_reloc_code_real_type
_get_imm15_reloc (struct mr32_operand_t *op, const enum mr32_mode mode)
{
  /* Default reloc, if we can't inspect the immediate value.  */
  bfd_reloc_code_real_type reloc = (mode == MR32_MODE_I15HL) ?
				       BFD_RELOC_MRISC32_I14 :
				       BFD_RELOC_MRISC32_I15;

  /* If we can inspect the constant we can check the range.  */
  if (op->exp.X_op == O_constant)
    {
      uint32_t value = (uint32_t) op->exp.X_add_number;
      if (mode == MR32_MODE_I15HL)
	{
	  if (_can_use_imm14lo (value))
	    reloc = BFD_RELOC_MRISC32_I14;
	  else if (_can_use_imm14hi (value))
	    reloc = BFD_RELOC_MRISC32_IHI14;
	  else
	    as_bad (_ ("14-bit immediate constant out of range: 0x%08x"), value);
	}
      else if (!_can_use_imm15 (value))
	as_bad (_ ("15-bit immediate constant out of range: 0x%08x"), value);
    }

  return reloc;
}

static bfd_reloc_code_real_type
_get_imm20_reloc (struct mr32_operand_t *op)
{
  // Default reloc, if we can't inspect the immediate value.
  bfd_reloc_code_real_type reloc = BFD_RELOC_MRISC32_I20;

  /* If we can inspect the constant we can check the range.  */
  if (op->exp.X_op == O_constant)
    {
      uint32_t value = (uint32_t) op->exp.X_add_number;
      if (_can_use_imm20lo (value))
	reloc = BFD_RELOC_MRISC32_I20;
      else if (_can_use_imm20hi (value))
	reloc = BFD_RELOC_MRISC32_IHI20;
      else
	as_bad (_ ("20-bit immediate constant out of range: 0x%08x"), value);
    }

  return reloc;
}

static bfd_boolean
_get_vec_mode_a (const mrisc32_opc_info_t *opcode,
		 const struct mr32_operand_t *op1,
		 const struct mr32_operand_t *op2,
		 const struct mr32_operand_t *op3, const bfd_boolean fold,
		 uint32_t *vec_mode)
{
  enum mr32_vector_conf vc;
  bfd_boolean can_fold = FALSE;

  /* Determine the vector operand configuration.  */
  if (op1->type == MR32_OT_VREG && op2->type == MR32_OT_VREG &&
      op3->type == MR32_OT_VREG)
    {
      vc = MR32_VC_VVV;
      can_fold = TRUE;
    }
  else if (op1->type == MR32_OT_VREG && op2->type == MR32_OT_VREG &&
	   op3->type == MR32_OT_SREG)
    {
      vc = MR32_VC_VVS;
    }
  else if (op1->type == MR32_OT_VREG && op2->type == MR32_OT_SREG &&
	   op3->type == MR32_OT_SREG)
    {
      vc = MR32_VC_VSS;
    }
  else if (op1->type == MR32_OT_VREG && op2->type == MR32_OT_SREG &&
	   op3->type == MR32_OT_VREG)
    {
      vc = MR32_VC_VSV;
    }
  else
    {
      vc = MR32_VC_NA;
    }

  /* Is folding requested but not allowed?  */
  if (fold && !can_fold)
    {
      as_bad (_ ("folding is not supported"));
      return FALSE;
    }

  /* Select vector mode.  */
  if (vc == MR32_VC_NA)
    {
      *vec_mode = 0;
      return TRUE;
    }
  else if (vc == opcode->vc2)
    {
      if (fold)
	{
	  as_bad (_ ("folding not supported"));
	  abort ();
	}
      *vec_mode = 2;
      return TRUE;
    }
  else if (vc == opcode->vc3)
    {
      *vec_mode = fold ? 1 : 3;
      return TRUE;
    }

  as_bad (_ ("invalid vector mode"));
  return FALSE;
}

static bfd_boolean
_get_vec_mode_bc (const mrisc32_opc_info_t *opcode,
		  const struct mr32_operand_t *op1,
		  const struct mr32_operand_t *op2, uint32_t *vec_mode)
{
  enum mr32_vector_conf vc;

  /* Determine the vector operand configuration.  */
  if (op1->type == MR32_OT_VREG && op2->type == MR32_OT_VREG)
    vc = MR32_VC_VVS;
  else if (op1->type == MR32_OT_VREG && op2->type == MR32_OT_SREG)
    vc = MR32_VC_VSS;
  else
    vc = MR32_VC_NA;

  /* Select vector mode.  */
  if (vc == MR32_VC_NA)
    {
      *vec_mode = 0;
      return TRUE;
    }
  else if (vc == opcode->vc2)
    {
      *vec_mode = 2;
      return TRUE;
    }

  as_bad (_ ("invalid vector mode"));
  return FALSE;
}

static const char *
_translate_alias (char *op_start, char *op_end, struct mr32_operand_t *op1,
		  struct mr32_operand_t *op2, struct mr32_operand_t *op3)
{
  /* Copy the op string to s_op_str.  */
  const int str_len = (int) (op_end - op_start);
  if (str_len > MAX_OP_STR_LEN)
    {
      as_bad (_ ("unsupported op code"));
      s_op_str[0] = 0;
      return &s_op_str[0];
    }
  memcpy (&s_op_str[0], op_start, (size_t) str_len);
  s_op_str[str_len] = 0;

  /* Convert known aliases to their canonical representation.  */
  if (str_len == 3 && (memcmp (&s_op_str[0], "nop", str_len) == 0))
    {
      /* Tranlaste "nop" to "or z, z, z".  */
      strcpy (&s_op_str[0], "or");
      _clear_operand (op1);
      op1->type = MR32_OT_SREG;
      op1->reg_no = 0;
      _clear_operand (op2);
      op2->type = MR32_OT_SREG;
      op2->reg_no = 0;
      _clear_operand (op3);
      op3->type = MR32_OT_SREG;
      op3->reg_no = 0;
    }
  else if (str_len == 3 && (memcmp (&s_op_str[0], "mov", str_len) == 0))
    {
      /* Tranlaste "mov x, y" to "or x, (v)z, y".  */
      strcpy (&s_op_str[0], "or");
      *op3 = *op2;
      _clear_operand (op2);
      op2->type = op1->type;
      op2->reg_no = 0;
    }
  else if (str_len == 1 && (memcmp (&s_op_str[0], "b", str_len) == 0))
    {
      /* Tranlaste "b #x" to "j pc, #x@pc".  */
      strcpy (&s_op_str[0], "j");
      *op3 = *op2;
      *op2 = *op1;
      op2->modifier = MR32_MOD_PC;
      _clear_operand (op1);
      op1->type = MR32_OT_SREG;
      op1->reg_no = MR32_REG_VPC;
    }
  else if (str_len == 2 && (memcmp (&s_op_str[0], "bl", str_len) == 0))
    {
      /* Tranlaste "bl #x" to "jl pc, #x@pc".  */
      strcpy (&s_op_str[0], "jl");
      *op3 = *op2;
      *op2 = *op1;
      op2->modifier = MR32_MOD_PC;
      _clear_operand (op1);
      op1->type = MR32_OT_SREG;
      op1->reg_no = MR32_REG_VPC;
    }
  else if (str_len == 1 && (memcmp (&s_op_str[0], "j", str_len) == 0) &&
	   (op2->type == MR32_OT_NONE))
    {
      /* Tranlaste "j x" to "j x, #0".  */
      char operand_str[3] = "#0";
      _decode_operand (&operand_str[0], op2);
    }
  else if (str_len == 2 && (memcmp (&s_op_str[0], "jl", str_len) == 0) &&
	   (op2->type == MR32_OT_NONE))
    {
      /* Tranlaste "jl x" to "jl x, #0".  */
      char operand_str[3] = "#0";
      _decode_operand (&operand_str[0], op2);
    }
  else if (str_len == 3 && (memcmp (&s_op_str[0], "ret", str_len) == 0) &&
	   (op1->type == MR32_OT_NONE) && (op2->type == MR32_OT_NONE))
    {
      /* Tranlaste "ret" to "j lr, #0".  */
      strcpy (&s_op_str[0], "j");
      _clear_operand (op1);
      op1->type = MR32_OT_SREG;
      op1->reg_no = MR32_REG_LR;
      char operand_str[3] = "#0";
      _decode_operand (&operand_str[0], op2);
    }
  else if (str_len == 3 && (memcmp (&s_op_str[0], "sub", str_len) == 0))
    {
      /* Tranlaste "sub a, b, c" to "sub a, c, b".  */
      /* The sub instruction actually implements "subtract second operand from
	 the third operand", but for readability and consistency with other CPU
	 ISA:s we reverse the operand order. */
      struct mr32_operand_t tmp = *op2;
      *op2 = *op3;
      *op3 = tmp;
    }
  else if (str_len == 3 && (memcmp (&s_op_str[0], "asr", str_len) == 0))
    {
      /* Tranlaste "asr a, b, c" to "ebf a, b, c".  */
      /* Note: 0 < c < 32, but we do not check that here.  */
      strcpy (&s_op_str[0], "ebf");
    }
  else if (str_len == 3 && (memcmp (&s_op_str[0], "lsr", str_len) == 0))
    {
      /* Tranlaste "lsr a, b, c" to "ebfu a, b, c".  */
      /* Note: 0 < c < 32, but we do not check that here.  */
      strcpy (&s_op_str[0], "ebfu");
    }
  else if (str_len == 3 && (memcmp (&s_op_str[0], "lsl", str_len) == 0))
    {
      /* Tranlaste "lsl a, b, c" to "mkbf a, b, c".  */
      /* Note: 0 < c < 32, but we do not check that here.  */
      strcpy (&s_op_str[0], "mkbf");
    }
  else if (str_len == 5 && (memcmp (&s_op_str[0], "getsr", str_len) == 0))
    {
      /* Tranlaste "getsr a, b" to "xchgsr a, z, b".  */
      strcpy (&s_op_str[0], "xchgsr");
      *op3 = *op2;
      _clear_operand (op2);
      op2->type = MR32_OT_SREG;
      op2->reg_no = MR32_REG_Z;
    }
  else if (str_len == 5 && (memcmp (&s_op_str[0], "setsr", str_len) == 0))
    {
      /* Tranlaste "setsr a, b" to "xchgsr z, a, b".  */
      strcpy (&s_op_str[0], "xchgsr");
      *op2 = *op1;
      _clear_operand (op1);
      op1->type = MR32_OT_SREG;
      op1->reg_no = MR32_REG_Z;
    }

  return &s_op_str[0];
}

static void
_emit_instruction (const char *op_str, struct mr32_operand_t *op1,
		   struct mr32_operand_t *op2, struct mr32_operand_t *op3,
		   bfd_boolean fold, enum mr32_packmode pack_mode,
		   enum mr32_bitmode bit_mode, enum mr32_selmode sel_mode,
		   enum mr32_widthmode width_mode)
{
  struct mr32_operand_t *reloc_operand;
  const mrisc32_opc_info_t *opcode_a;
  const mrisc32_opc_info_t *opcode_b;
  const mrisc32_opc_info_t *opcode_c;
  const mrisc32_opc_info_t *opcode_d;
  const mrisc32_opc_info_t *opcode_e;
  const mrisc32_opc_info_t *opcode;
  uint32_t iword = 0u;
  uint32_t reg1_enc;
  uint32_t reg2_enc;
  uint32_t reg3_enc;
  uint32_t vec_mode;

  int k;
  int pc_rel;
  unsigned long where;
  bfd_reloc_code_real_type reloc_type;

  /* Reserve space for the instruction (it's always 4 bytes = 32 bits).  */
  char *p = frag_more (4);

  /* Look up the matching op code for each format type (there must be at least
     one match).  */
  opcode_a = (mrisc32_opc_info_t *) str_hash_find (s_opc_type_a_map, op_str);
  opcode_b = NULL;
  for (k = 0; opcode_b == NULL && k < 4; ++k)
    {
      opcode_b = (mrisc32_opc_info_t *) str_hash_find (s_opc_type_b_map[k], op_str);
    }
  opcode_c = (mrisc32_opc_info_t *) str_hash_find (s_opc_type_c_map, op_str);
  opcode_d = (mrisc32_opc_info_t *) str_hash_find (s_opc_type_d_map, op_str);
  opcode_e = (mrisc32_opc_info_t *) str_hash_find (s_opc_type_e_map, op_str);
  if (opcode_a == NULL && opcode_b == NULL && opcode_c == NULL &&
      opcode_d == NULL && opcode_e == NULL)
    {
      as_bad (_ ("unknown opcode %s"), op_str);
      return;
    }

  /* Determine the format (A, B, C, D or E) based on the operand types.  */
  if (opcode_a && ((_is_reg (op1) && _is_reg (op2) && _is_reg (op3))))
    {
      opcode = opcode_a;
    }
  else if (opcode_b &&
	   ((opcode_b->mode != MR32_MODE_NONE &&
	     _is_reg (op1) &&
	     _is_reg (op2) &&
	     op3->type == MR32_OT_NONE) ||
	    (opcode_b->mode == MR32_MODE_NONE &&
	     op1->type == MR32_OT_NONE &&
	     op2->type == MR32_OT_NONE &&
	     op3->type == MR32_OT_NONE)))
    {
      opcode = opcode_b;
    }
  else if (opcode_c && (_is_reg (op1) && _is_reg (op2) && _is_imm (op3)))
    {
      opcode = opcode_c;
    }
  else if (opcode_d &&
	   (_is_reg (op1) && _is_imm (op2) && op3->type == MR32_OT_NONE))
    {
      opcode = opcode_d;
    }
  else if (opcode_e &&
	   (_is_reg (op1) && _is_imm (op2) && op3->type == MR32_OT_NONE))
    {
      opcode = opcode_e;
    }
  else
    {
      as_bad (_ ("unsupported operands"));
      return;
    }

  /* Default: No reloc.  */
  pc_rel = 0;
  where = 0;
  reloc_type = BFD_RELOC_NONE;

  /* Constuct the instruction word.  */
  switch (opcode->format)
    {
    case MR32_FMT_A:
      {
	/* Encode the operation.  */
	iword = (uint32_t) opcode->op;

	/* Encode the registers.  */
	if (!(_encode_reg (op1, FALSE, &reg1_enc) &&
	      _encode_reg (op2, FALSE, &reg2_enc) &&
	      _encode_reg (op3, FALSE, &reg3_enc)))
	  {
	    as_bad (_ ("invalid register operand"));
	    return;
	  }
	iword |= reg1_enc << 21;
	iword |= reg2_enc << 16;
	iword |= reg3_enc << 9;

	/* Encode the vector mode.  */
	if (!_get_vec_mode_a (opcode, op1, op2, op3, fold, &vec_mode))
	  return;
	iword |= vec_mode << 14;

	/* Encode the packed mode.  */
	if (pack_mode != MR32_PM_NONE)
	  {
	    if (opcode->ft != MR32_FT_PACK)
	      {
		as_bad (_ ("packed mode not supported"));
		return;
	      }
	    iword |= ((uint32_t) pack_mode) << 7;
	  }

	/* Encode the register scale factor.  */
	if (op3->reg_scale != MR32_RS_NONE)
	  {
	    if (opcode->ft != MR32_FT_SCALE)
	      {
		as_bad (_ ("register scale not supported"));
		return;
	      }
	    iword |= ((uint32_t) op3->reg_scale) << 7;
	  }

	/* Encode the bitwise negation mode.  */
	if (bit_mode != MR32_BM_NONE)
	  {
	    if (opcode->ft != MR32_FT_BIT)
	      {
		as_bad (_ ("bitwise negation mode not supported"));
		return;
	      }
	    iword |= ((uint32_t) bit_mode) << 7;
	  }

	/* Encode the SEL mode.  */
	if (sel_mode != MR32_SM_NONE)
	  {
	    if (opcode->ft != MR32_FT_SEL)
	      {
		as_bad (_ ("sel mode not supported"));
		return;
	      }
	    iword |= ((uint32_t) sel_mode) << 7;
	  }

	/* Encode the width mode.  */
	if (width_mode != MR32_WM_NONE)
	  {
	    if (opcode->ft != MR32_FT_WIDTH)
	      {
		as_bad (_ ("width mode not supported"));
		return;
	      }
	    iword |= ((uint32_t) width_mode) << 7;
	  }

	break;
      }

    case MR32_FMT_B:
      {
	/* Encode the operation.  */
	iword = (uint32_t) opcode->op;

	/* Encode the registers.  */
	if (opcode->mode == MR32_MODE_NONE)
	  {
	    reg1_enc = 0;
	    reg2_enc = 0;
	  }
	else if (!(_encode_reg (op1, FALSE, &reg1_enc) &&
		   _encode_reg (op2, FALSE, &reg2_enc)))
	  {
	    as_bad (_ ("invalid register operand"));
	    return;
	  }
	iword |= reg1_enc << 21;
	iword |= reg2_enc << 16;

	/* Encode the function.  */
	iword |= ((uint32_t) opcode->func) << 9;

	/* Encode the vector mode.  */
	if (!_get_vec_mode_bc (opcode, op1, op2, &vec_mode))
	  return;
	if (fold)
	  {
	    as_bad (_ ("folding not supported"));
	    return;
	  }
	iword |= vec_mode << 14;

	/* Encode the packed mode.  */
	if (pack_mode != MR32_PM_NONE)
	  {
	    if (opcode->ft != MR32_FT_PACK)
	      {
		as_bad (_ ("packed mode not supported"));
		return;
	      }
	    iword |= ((uint32_t) pack_mode) << 7;
	  }

	/* Encode the width mode.  */
	if (width_mode != MR32_WM_NONE)
	  {
	    if (opcode->ft != MR32_FT_WIDTH)
	      {
		as_bad (_ ("width mode not supported"));
		return;
	      }
	    iword |= ((uint32_t) width_mode) << 7;
	  }

	break;
      }

    case MR32_FMT_C:
      {
	/* Check conditions for immediate mode instructions.  */
	if (pack_mode != MR32_PM_NONE)
	  {
	    as_bad (_ ("packed mode not supported"));
	    return;
	  }
	if (bit_mode != MR32_BM_NONE)
	  {
	    as_bad (_ ("bitwise negation mode not supported"));
	    return;
	  }
	if (sel_mode != MR32_SM_NONE)
	  {
	    as_bad (_ ("sel mode not supported"));
	    return;
	  }
	if (width_mode != MR32_WM_NONE)
	  {
	    as_bad (_ ("width mode not supported"));
	    return;
	  }
	if (fold)
	  {
	    as_bad (_ ("folding not supported"));
	    return;
	  }

	/* Encode the operation.  */
	iword = ((uint32_t) opcode->op) << 26;

	/* Encode the registers.  */
	if (!(_encode_reg (op1, FALSE, &reg1_enc) &&
	      _encode_reg (op2, FALSE, &reg2_enc)))
	  {
	    as_bad (_ ("invalid register operand"));
	    return;
	  }
	iword |= reg1_enc << 21;
	iword |= reg2_enc << 16;

	/* Encode the vector mode.  */
	if (!_get_vec_mode_bc (opcode, op1, op2, &vec_mode))
	  return;
	iword |= vec_mode << 14;

	/* Handle the immediate value (relocation information).  */
	reloc_operand = op3;
	where = (unsigned long) (p - frag_now->fr_literal);
	switch (reloc_operand->modifier)
	  {
	  case MR32_MOD_NONE:
	    reloc_type = _get_imm15_reloc (reloc_operand, opcode->mode);
	    break;

	  case MR32_MOD_LO:
	    reloc_type = BFD_RELOC_MRISC32_ILO13;
	    break;

	  case MR32_MOD_PCLO:
	    reloc_type = BFD_RELOC_MRISC32_PCREL_LO11;
	    pc_rel = 1;
	    break;

	  default:
	    as_bad (_ ("unsupported modifier"));
	    return;
	  }

	break;
      }

    case MR32_FMT_D:
      {
	/* Check that this is a pure scalar operation.  */
	if (op1->type == MR32_OT_VREG)
	  {
	    as_bad (_ ("vector operation not supported"));
	    return;
	  }
	if (pack_mode != MR32_PM_NONE)
	  {
	    as_bad (_ ("packed mode not supported"));
	    return;
	  }
	if (bit_mode != MR32_BM_NONE)
	  {
	    as_bad (_ ("bitwise negation mode not supported"));
	    return;
	  }
	if (sel_mode != MR32_SM_NONE)
	  {
	    as_bad (_ ("sel mode not supported"));
	    return;
	  }
	if (width_mode != MR32_WM_NONE)
	  {
	    as_bad (_ ("width mode not supported"));
	    return;
	  }
	if (fold)
	  {
	    as_bad (_ ("folding not supported"));
	    return;
	  }

	/* Encode the operation.  */
	iword = (((uint32_t) opcode->op) << 26) | 0xc0000000u;

	/* Encode the register.  */
	if (!_encode_reg (op1, _is_reg31_pc (opcode), &reg1_enc))
	  {
	    as_bad (_ ("invalid register operand"));
	    return;
	  }
	iword |= reg1_enc << 21;

	/* Handle the immediate value (relocation information).  */
	reloc_operand = op2;
	where = (unsigned long) (p - frag_now->fr_literal);
	switch (opcode->mode)
	  {
	  case MR32_MODE_I21HL:
	    if (reloc_operand->modifier == MR32_MOD_NONE)
	      {
		reloc_type = _get_imm20_reloc (reloc_operand);
	      }
	    else if (reloc_operand->modifier == MR32_MOD_HI)
	      {
		reloc_type = BFD_RELOC_MRISC32_IHI19;
	      }
	    else
	      {
		as_bad (_ ("unsupported modifier"));
		return;
	      }
	    break;

	  case MR32_MODE_REGREL21x4:
	    if (reloc_operand->modifier == MR32_MOD_NONE)
	      {
		reloc_type = BFD_RELOC_MRISC32_I21X4;
	      }
	    else if (reloc_operand->modifier == MR32_MOD_LO)
	      {
		reloc_type = BFD_RELOC_MRISC32_ILO11X4;
	      }
	    else if (reloc_operand->modifier == MR32_MOD_PC)
	      {
		reloc_type = BFD_RELOC_MRISC32_PCREL_21X4;
		pc_rel = 1;
	      }
	    else if (reloc_operand->modifier == MR32_MOD_PCLO)
	      {
		reloc_type = BFD_RELOC_MRISC32_PCREL_LO9X4;
		pc_rel = 1;
	      }
	    else
	      {
		as_bad (_ ("unsupported modifier"));
		return;
	      }
	    break;

	  case MR32_MODE_PCREL21x4:
	    if (reloc_operand->modifier == MR32_MOD_PC)
	      {
		reloc_type = BFD_RELOC_MRISC32_PCREL_21X4;
		pc_rel = 1;
	      }
	    else
	      {
		as_bad (_ ("unsupported modifier"));
		return;
	      }
	    break;

	  case MR32_MODE_PCREL21H:
	    if (reloc_operand->modifier == MR32_MOD_PCHI)
	      {
		reloc_type = BFD_RELOC_MRISC32_PCREL_HI21;
		pc_rel = 1;
	      }
	    else
	      {
		as_bad (_ ("unsupported modifier"));
		return;
	      }
	    break;

	  default:
	    abort ();
	  }

	break;
      }


    case MR32_FMT_E:
      {
	/* Check that this is a pure scalar operation.  */
	if (op1->type == MR32_OT_VREG)
	  {
	    as_bad (_ ("vector operation not supported"));
	    return;
	  }
	if (pack_mode != MR32_PM_NONE)
	  {
	    as_bad (_ ("packed mode not supported"));
	    return;
	  }
	if (bit_mode != MR32_BM_NONE)
	  {
	    as_bad (_ ("bitwise negation mode not supported"));
	    return;
	  }
	if (sel_mode != MR32_SM_NONE)
	  {
	    as_bad (_ ("sel mode not supported"));
	    return;
	  }
	if (width_mode != MR32_WM_NONE)
	  {
	    as_bad (_ ("width mode not supported"));
	    return;
	  }
	if (fold)
	  {
	    as_bad (_ ("folding not supported"));
	    return;
	  }

	/* Encode the operation.  */
	iword = (((uint32_t) opcode->op) << 18) | 0xdc000000u;

	/* Encode the register.  */
	if (!_encode_reg (op1, FALSE, &reg1_enc))
	  {
	    as_bad (_ ("invalid register operand"));
	    return;
	  }
	iword |= reg1_enc << 21;

	/* Handle the immediate value (relocation information).  */
	reloc_operand = op2;
	where = (unsigned long) (p - frag_now->fr_literal);
	switch (opcode->mode)
	  {
	  case MR32_MODE_PCREL18x4:
	    if (reloc_operand->modifier == MR32_MOD_NONE)
	      {
		reloc_type = BFD_RELOC_MRISC32_PCREL_18X4;
		pc_rel = 1;
	      }
	    else
	      {
		as_bad (_ ("unsupported modifier"));
		return;
	      }
	    break;

	  default:
	    abort ();
	  }

	break;
      }

    default:
      abort ();
      break;
    }

  /* Add a reloc?  */
  if (reloc_type != BFD_RELOC_NONE)
    {
      gas_assert (reloc_operand != NULL);
      reloc_howto_type *howto = bfd_reloc_type_lookup (stdoutput, reloc_type);
      if (howto == NULL)
	as_bad (_ ("Unsupported MRISC32 relocation number %d"), reloc_type);
      fix_new_exp (frag_now, where, bfd_get_reloc_size (howto),
		   &reloc_operand->exp, pc_rel, reloc_type);
    }

  /* Emit the instruction.  */
  md_number_to_chars (p, iword, 4);
}

static bfd_boolean
_expand_alias (const char *op_str, struct mr32_operand_t *op1,
	       struct mr32_operand_t *op2, struct mr32_operand_t *op3,
	       bfd_boolean fold, enum mr32_packmode pack_mode,
	       enum mr32_bitmode bit_mode, enum mr32_selmode sel_mode,
	       enum mr32_widthmode width_mode)
{
  /* Expand "ldi" to suitable instruction combinations.  */
  if (strcmp (op_str, "ldi") == 0)
    {
      if (op1->type != MR32_OT_SREG || op2->type != MR32_OT_EXPRESSION ||
	  op3->type != MR32_OT_NONE)
	{
	  as_bad (_ ("invalid operands"));
	  return TRUE;
	}

      if (fold || pack_mode != MR32_PM_NONE)
	{
	  as_bad (_ ("invalid mode"));
	  return TRUE;
	}

      if (op2->exp.X_op == O_constant)
	{
	  if (op2->modifier == MR32_MOD_NONE)
	    {
	      uint32_t value = (uint32_t) op2->exp.X_add_number;
	      if (_can_use_imm20lo (value) || _can_use_imm20hi (value))
		_emit_instruction ("ldi", op1, op2, op3, fold, pack_mode, bit_mode, sel_mode, width_mode);
	      else
		{
		  op2->modifier = MR32_MOD_HI;
		  _emit_instruction ("ldi", op1, op2, op3, fold, pack_mode, bit_mode, sel_mode, width_mode);
		  op2->modifier = MR32_MOD_LO;
		  _emit_instruction ("or", op1, op1, op2, fold, pack_mode, bit_mode, sel_mode, width_mode);
		}
	    }
	  else
	    {
	      as_bad (_ ("invalid modifier"));
	      return TRUE;
	    }
	}
      else
	{
	  if (op2->modifier == MR32_MOD_PC)
	    {
	      op2->modifier = MR32_MOD_PCHI;
	      _emit_instruction ("addpchi", op1, op2, op3, fold, pack_mode, bit_mode, sel_mode, width_mode);
	      op2->modifier = MR32_MOD_PCLO;
	      op2->exp.X_add_number += 4;
	      _emit_instruction ("add", op1, op1, op2, fold, pack_mode, bit_mode, sel_mode, width_mode);
	    }
	  else if (op2->modifier == MR32_MOD_NONE)
	    {
	      op2->modifier = MR32_MOD_HI;
	      _emit_instruction ("ldi", op1, op2, op3, fold, pack_mode, bit_mode, sel_mode, width_mode);
	      op2->modifier = MR32_MOD_LO;
	      _emit_instruction ("or", op1, op1, op2, fold, pack_mode, bit_mode, sel_mode, width_mode);
	    }
	  else
	    {
	      /* Just emit a regular LDI instruction and handle modifiers in reloc.  */
	      _emit_instruction ("ldi", op1, op2, op3, fold, pack_mode, bit_mode, sel_mode, width_mode);
	      return TRUE;
	    }
	}

      return TRUE;
    }

  /* Expand "call" and "tail" to suitable instruction combinations.  */
  if (strcmp (op_str, "call") == 0 || strcmp (op_str, "tail") == 0)
    {
      /* Handle differences between call and tail.  */
      bfd_boolean is_call = (strcmp (op_str, "call") == 0);
      const char *jump_insn = (is_call ? "jl" : "j");

      /* For "call" we use LR as a scratch register. This is safe,
         since LR will be overwritten by JL anyway.
         For "tail" we use R15 as a scratch register. This is safe, since
	 R15 is neither an argument register nor a callee saved register.  */
      int scratch_reg_no = (is_call ? MR32_REG_LR : 15);

      if (op1->type != MR32_OT_EXPRESSION || op2->type != MR32_OT_NONE ||
	  op3->type != MR32_OT_NONE)
	{
	  as_bad (_ ("invalid operands"));
	  return TRUE;
	}

      if (fold || pack_mode != MR32_PM_NONE)
	{
	  as_bad (_ ("invalid mode"));
	  return TRUE;
	}

      /* Put the target address in operand 2.  */
      *op2 = *op1;

      /* Use operand 1 as a scratch register.  */
      _clear_operand (op1);
      op1->type = MR32_OT_SREG;
      op1->reg_no = scratch_reg_no;

      if (op2->modifier == MR32_MOD_PC)
	{
	  op2->modifier = MR32_MOD_PCHI;
	  _emit_instruction ("addpchi", op1, op2, op3, fold, pack_mode, bit_mode, sel_mode, width_mode);
	  op2->modifier = MR32_MOD_PCLO;
	  op2->exp.X_add_number += 4;
	  _emit_instruction (jump_insn, op1, op2, op3, fold, pack_mode, bit_mode, sel_mode, width_mode);
	}
      else if (op2->modifier == MR32_MOD_NONE)
	{
	  op2->modifier = MR32_MOD_HI;
	  _emit_instruction ("ldhi", op1, op2, op3, fold, pack_mode, bit_mode, sel_mode, width_mode);
	  op2->modifier = MR32_MOD_LO;
	  _emit_instruction (jump_insn, op1, op2, op3, fold, pack_mode, bit_mode, sel_mode, width_mode);
	}
      else
	{
	  as_bad (_ ("invalid address modifier"));
	}

      return TRUE;
    }

  /* The instruction was not expanded.  */
  return FALSE;
}

void
md_operand (expressionS *op ATTRIBUTE_UNUSED)
{
  /* Empty for now. */
}

/* This function is called once, at assembler startup time.  It sets
   up the hash table with all the opcodes in it, and also initializes
   some aliases for compatibility with other assemblers.  */
void
md_begin (void)
{
  int count, k;

  /* Insert type A opcodes into a hash table.  */
  s_opc_type_a_map = str_htab_create ();
  for (count = 0; count < 124; ++count)
    {
      const mrisc32_opc_info_t *opcode = &mrisc32_opc_type_a_info[count];
      if (opcode->format != MR32_FMT_BAD)
	str_hash_insert (s_opc_type_a_map, opcode->name, (void *) opcode, 0);
    }

  /* Insert type B opcodes into a hash table.  */
  for (k = 0; k < 4; ++k)
    {
      s_opc_type_b_map[k] = str_htab_create ();
      for (count = 0; count < 64; ++count)
	{
	  const mrisc32_opc_info_t *opcode = &mrisc32_opc_type_b_info[k][count];
	  if (opcode->format != MR32_FMT_BAD)
	    str_hash_insert (s_opc_type_b_map[k], opcode->name, (void *) opcode, 0);
	}
    }

  /* Insert type C opcodes into a hash table.  */
  s_opc_type_c_map = str_htab_create ();
  for (count = 0; count < 48; ++count)
    {
      const mrisc32_opc_info_t *opcode = &mrisc32_opc_type_c_info[count];
      if (opcode->format != MR32_FMT_BAD)
	str_hash_insert (s_opc_type_c_map, opcode->name, (void *) opcode, 0);
    }

  /* Insert type D opcodes into a hash table.  */
  s_opc_type_d_map = str_htab_create ();
  for (count = 0; count < 7; ++count)
    {
      const mrisc32_opc_info_t *opcode = &mrisc32_opc_type_d_info[count];
      if (opcode->format != MR32_FMT_BAD)
	str_hash_insert (s_opc_type_d_map, opcode->name, (void *) opcode, 0);
    }

  /* Insert type E opcodes into a hash table.  */
  s_opc_type_e_map = str_htab_create ();
  for (count = 0; count < 8; ++count)
    {
      const mrisc32_opc_info_t *opcode = &mrisc32_opc_type_e_info[count];
      if (opcode->format != MR32_FMT_BAD)
	str_hash_insert (s_opc_type_e_map, opcode->name, (void *) opcode, 0);
    }

  bfd_set_arch_mach (stdoutput, TARGET_ARCH, 0);
}


/* This is the guts of the machine-dependent assembler.  STR points to
   a machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.  */
void
md_assemble (char *str)
{
  char *op_start;
  char *op_end;
  const char *op_str;
  char *suffix_start;
  char *suffix_end;
  char *operand_start;
  char *operands_end;

  enum mr32_packmode pack_mode;
  enum mr32_bitmode bit_mode;
  enum mr32_selmode sel_mode;
  enum mr32_widthmode width_mode;
  bfd_boolean fold;
  struct mr32_operand_t operand1;
  struct mr32_operand_t operand2;
  struct mr32_operand_t operand3;

  /* Drop leading whitespace.  */
  while (ISSPACE (*str))
    ++str;

  /* Find the op code end.  */
  op_start = str;
  for (op_end = op_start;
       !is_end_of_line[(unsigned char) *op_end] && !ISSPACE (*op_end) &&
       (*op_end != '.') && (*op_end != '/') && (*op_end != '_');
       ++op_end)
    ;
  if (op_end == op_start)
    {
      as_bad (_ ("can't find opcode"));
      return;
    }

  /* Parse the op code suffix (e.g. '.b', '.h/f', etc).  */
  suffix_start = op_end;
  suffix_end = suffix_start;
  if ((*suffix_start == '.') || (*suffix_start == '/') || (*suffix_start == '_'))
    {
      for (; !is_end_of_line[(unsigned char) *suffix_end] &&
	     !ISSPACE (*suffix_end);
	   ++suffix_end)
	;
    }
  if (!_decode_suffix (suffix_start, suffix_end, &pack_mode, &bit_mode, &sel_mode, &width_mode, &fold))
    {
      return;
    }

  /* Find the start of the operands.  */
  for (operand_start = suffix_end;
       !is_end_of_line[(unsigned char) *operand_start] &&
       ISSPACE (*operand_start);
       ++operand_start)
    ;

  /* Decode the operands (there are max three operands).  */
  operand_start = _decode_operand (operand_start, &operand1);
  if (*operand_start == '[')
    {
      operands_end = _decode_address_operands (operand_start, &operand2,
					       &operand3);
    }
  else
    {
      operand_start = _decode_operand (operand_start, &operand2);
      operands_end = _decode_operand (operand_start, &operand3);
    }
  if (operand1.type == MR32_OT_BAD || operand2.type == MR32_OT_BAD ||
      operand3.type == MR32_OT_BAD)
    return;

  /* Check the end of the line (mostly to warn about trailing garbage).  */
  while (ISSPACE (*operands_end))
    ++operands_end;
  if (*operands_end != 0)
    as_warn (_ ("extra stuff on line ignored"));

  /* We only support register scaling for the third operand.  */
  if (operand1.reg_scale != MR32_RS_NONE || operand2.reg_scale != MR32_RS_NONE)
    {
      as_bad (_ ("register scaling only supported for the third operand"));
      return;
    }

  /* Handle aliases.  After this call, op_str contains to the correct op string,
     and operand1, operand2 and operand3 may have been adjusted.  */
  op_str = _translate_alias (op_start, op_end, &operand1, &operand2, &operand3);

  /* Some aliases expand to more than one instruction. If the instruction gets
     expanded, we're done. */
  if (_expand_alias (op_str, &operand1, &operand2, &operand3, fold, pack_mode, bit_mode, sel_mode, width_mode))
    return;

  /* Encode and emit the instruction.  */
  _emit_instruction (op_str, &operand1, &operand2, &operand3, fold, pack_mode, bit_mode, sel_mode, width_mode);
}

const char *
md_atof (int type, char *litP, int *sizeP)
{
  return ieee_md_atof (type, litP, sizeP, FALSE);
}

int
md_parse_option (int c, const char *arg ATTRIBUTE_UNUSED)
{
  switch (c)
    {
    case OPTION_RELAX:
      s_mrisc32_relax = TRUE;
      break;

    case OPTION_NO_RELAX:
      s_mrisc32_relax = FALSE;
      break;

    default:
      return 0;
    }

  return 1;
}

void
md_show_usage (FILE *stream)
{
  fprintf (stream, _ ("\
MRISC32 options:\n\
  -mrelax                 enable relax (default)\n\
  -mno-relax              disable relax\n\
"));
}

/* Apply a fixup to the object file.  */
void
md_apply_fix (fixS *fixP, valueT *valP, segT seg ATTRIBUTE_UNUSED)
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  int32_t val = (int32_t) *valP;
  uint32_t word;

  /* Remember value for tc_gen_reloc.  */
  fixP->fx_addnumber = *valP;

  /* TODO(m): Many (all?) of these should be skipped if fx_done == 0 (see
     below).  */
  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_32:
      md_number_to_chars (buf, *valP, 4);
      break;

    case BFD_RELOC_16:
      md_number_to_chars (buf, *valP, 2);
      break;

    case BFD_RELOC_8:
      md_number_to_chars (buf, *valP, 1);
      break;

    case BFD_RELOC_MRISC32_I14:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) (val & 0x3fffu);
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    case BFD_RELOC_MRISC32_IHI14:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) ((val >> 18) & 0x3fffu) | 0x4000;
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    case BFD_RELOC_MRISC32_I15:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) (val & 0x7fffu);
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    case BFD_RELOC_MRISC32_I20:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) (val & 0xfffffu);
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    case BFD_RELOC_MRISC32_IHI19:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) ((val >> 12) & 0xffffeu) | 0x100000;
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    case BFD_RELOC_MRISC32_IHI20:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) ((val >> 12) & 0xfffffu) | 0x100000;
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    case BFD_RELOC_MRISC32_PCREL_HI21:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) ((val >> 11) & 0x1fffffu);
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    case BFD_RELOC_MRISC32_ILO13:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) (val & 0x1fffu);
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    case BFD_RELOC_MRISC32_PCREL_LO11:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) (val & 0x7ffu);
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    case BFD_RELOC_MRISC32_ILO11X4:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) ((val >> 2) & 0x7ff);
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    case BFD_RELOC_MRISC32_PCREL_LO9X4:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) ((val >> 2) & 0x1ff);
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    case BFD_RELOC_MRISC32_PCREL_18X4:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) ((val >> 2) & 0x3ffff);
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    case BFD_RELOC_MRISC32_I21X4:
    case BFD_RELOC_MRISC32_PCREL_21X4:
      word = (uint32_t) bfd_getl32 (buf);
      word |= (uint32_t) ((val >> 2) & 0x1fffff);
      bfd_putl32 ((bfd_vma) word, buf);
      break;

    default:
      abort ();
    }

  if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;
}

/* Put number into target byte order.  */
void
md_number_to_chars (char *ptr, valueT use, int nbytes)
{
  number_to_chars_littleendian (ptr, use, nbytes);
}

/* Generate a machine-dependent relocation.  */
arelent *
tc_gen_reloc (asection *section, fixS *fixP)
{
  arelent *reloc;

  reloc = XNEW (arelent);
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixP->fx_r_type);
  if (reloc->howto == NULL)
    {
      as_bad_where (fixP->fx_file, fixP->fx_line,
		    _ ("internal error: can't export reloc type %d (`%s')"),
		    fixP->fx_r_type, bfd_get_reloc_code_name (fixP->fx_r_type));
      return NULL;
    }

  gas_assert (!fixP->fx_pcrel == !reloc->howto->pc_relative);

  reloc->sym_ptr_ptr = XNEW (asymbol *);
  *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixP->fx_addsy);
  reloc->address = fixP->fx_frag->fr_address + fixP->fx_where;

  if (fixP->fx_pcrel)
    {
      if (section->use_rela_p)
	fixP->fx_offset -= md_pcrel_from_section (fixP, section);
      else
	fixP->fx_offset = reloc->address;
    }
  reloc->addend = fixP->fx_offset;

  if (fixP->fx_r_type == BFD_RELOC_VTABLE_ENTRY)
    reloc->address = fixP->fx_offset;

  return reloc;
}

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */
long
md_pcrel_from_section (fixS *fixP, segT sec)
{
  if (fixP->fx_addsy != (symbolS *) NULL &&
      (!S_IS_DEFINED (fixP->fx_addsy) ||
       (S_GET_SEGMENT (fixP->fx_addsy) != sec) ||
       S_IS_EXTERNAL (fixP->fx_addsy) || S_IS_WEAK (fixP->fx_addsy)))
    {
      /* The symbol is undefined (or is defined but not in this section).
	 Let the linker figure it out.  */
      return 0;
    }

  return fixP->fx_frag->fr_address + fixP->fx_where;
}

static const pseudo_typeS
mrisc32_pseudo_table[] =
{
  {"half", cons, 2},
  {"word", cons, 4},
  {NULL, NULL, 0}
};

void
mrisc32_pop_insert (void)
{
  pop_insert (mrisc32_pseudo_table);
  return;
}
