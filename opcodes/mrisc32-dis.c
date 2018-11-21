/* Disassemble MRISC32 instructions.
   Copyright (C) 2018 Free Software Foundation, Inc.
   Contributed by Marcus Geelnard (m@bitsnbites.eu)

   Based on the moxie disassembler.

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include <stdio.h>

#define STATIC_TABLE
#define DEFINE_TABLE

#include "disassemble.h"
#include "opcode/mrisc32.h"

static const char * scalar_regs[32] =
  { "z", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
    "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
    "r24", "r25", "r26", "tp", "fp", "sp", "lr", "vl" };

static const char * vector_regs[32] =
  { "vz", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
    "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15",
    "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",
    "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31" };

/* Functions to extract operands from the instruction word.  */
static int
_get_reg1 (const uint32_t iw)
{
  return (int) ((iw >> 21) & 31);
}

static int
_get_reg2 (const uint32_t iw)
{
  return (int) ((iw >> 16) & 31);
}

static int
_get_reg3 (const uint32_t iw)
{
  return (int) ((iw >> 9) & 31);
}

static int
_get_func (const uint32_t iw)
{
  return (int) ((iw >> 9) & 63);
}

static int32_t
_get_imm15 (const uint32_t iw, const enum mr32_mode mode)
{
  switch (mode)
    {
    default:
    case MR32_MODE_I15HL:
      if (iw & 0x4000U)
	return (int32_t) (((iw & 0x3fffU) << 18) | ((iw & 1) ? 0x3ffffU : 0));
      else
	return (int32_t) ((iw & 0x3fffU) | ((iw & 0x2000U) ? 0xffffc000U : 0));
      break;

    case MR32_MODE_I15_ADDR:
      return (int32_t) ((iw & 0x7fffU) | ((iw & 0x4000U) ? 0xffff8000U : 0));
    }
}

static int32_t
_get_imm18 (const uint32_t iw)
{
  return (int32_t) ((iw & 0x0003ffff) | ((iw & 0x00020000) ? 0xfffc0000 : 0));
}

static int32_t
_get_imm21 (const uint32_t iw)
{
  return (int32_t) ((iw & 0x001fffff) | ((iw & 0x00100000) ? 0xffe00000 : 0));
}

static int32_t
_get_imm21h (const uint32_t iw)
{
  return (int32_t) ((iw & 0x001fffff) << 11);
}

static int32_t
_get_imm21hl (const uint32_t iw)
{
  if (iw & 0x100000U)
    return (int32_t) (((iw & 0xfffffU) << 12) | ((iw & 1) ? 0xfffU : 0));
  else
    return (int32_t) ((iw & 0xfffffU) | ((iw & 0x80000U) ? 0xfff00000U : 0));;
}

/* Get opcode suffix.  */
static const char *
_get_suffix (const unsigned vec_mode, const unsigned pack_mode,
	     const unsigned bit_mode, const unsigned sel_mode)
{
  static const char* PACK_SUFFIX[2][4] = {
    {"", ".b", ".h", ".?"},	    /* Non-folding.  */
    {"/f", ".b/f", ".h/f", ".?/f"}  /* Folding.  */
  };

  static const char* BIT_SUFFIX[2][4] = {
    {"", ".pn", ".np", ".nn"},	       /* Non-folding.  */
    {"/f", ".pn/f", ".np/f", ".nn/f"}  /* Folding.  */
  };

  static const char* SEL_SUFFIX[2][4] = {
    {"", ".132", ".213", ".231"},	  /* Non-folding.  */
    {"/f", ".132/f", ".213/f", ".231/f"}  /* Folding.  */
  };

  const int folding = (vec_mode == 1 ? 1 : 0);
  if (pack_mode > 0 && pack_mode <= 3)
    return PACK_SUFFIX[folding][pack_mode];
  if (bit_mode > 0 && bit_mode <= 3)
    return BIT_SUFFIX[folding][bit_mode];
  else if (sel_mode <= 3)
    return SEL_SUFFIX[folding][sel_mode];
  else
    return "?";
}

static bool
_is_scalar (const mrisc32_opc_info_t *opcode, const unsigned vec_mode,
            const int arg_no)
{
  enum mr32_vector_conf vc;

  if (vec_mode == 0)
    return true;

  /* Determine vector argument configuration.  */
  vc = MR32_VC_NA;
  if (vec_mode == 2)
    vc = opcode->vc2;
  else if (vec_mode == 3 || vec_mode == 1)
    vc = opcode->vc3;

  /* Is the given argument number (1..3) a scalar argument?  */
  switch (vc)
    {
    default:
    case MR32_VC_NA:
      return true;

    case MR32_VC_VVV:
      return false;

    case MR32_VC_VVS:
      return (arg_no == 3) ? true : false;

    case MR32_VC_VSS:
      return (arg_no != 1) ? true : false;

    case MR32_VC_VSV:
      return (arg_no == 2) ? true : false;
    }
}

static bool
_has_address_operands (const mrisc32_opc_info_t *opcode)
{
  return opcode->mode == MR32_MODE_3REG_ADDR || opcode->mode == MR32_MODE_I15_ADDR;
}

static void
_print_full_mnemonic (struct disassemble_info *info, const char *name,
                      const char *suffix)
{
  size_t len = strlen (name) + strlen (suffix) + 1u;
  info->fprintf_func (info->stream, "%s%s ", name, suffix);
  for (; len < 8u; ++len)
    {
      info->fprintf_func (info->stream, " ");
    }
}

static bool
_should_swap_operands_2_and_3 (const mrisc32_opc_info_t *opcode)
{
  /* sub a, b, c  ->  sub a, c, b */
  return (opcode->op == 0x17u) ? true : false;
}

static void
_print_shuf_comment (struct disassemble_info *info, const int32_t ctrl)
{
  void *stream = info->stream;
  fprintf_ftype fpr = info->fprintf_func;
  fpr (stream, "  ; ");
  switch (ctrl)
    {
    case 2336:
      fpr (stream, "Zero extend byte");
      break;
    case 2888:
      fpr (stream, "Zero extend half-word");
      break;
    case 6432:
      fpr (stream, "Sign extend byte");
      break;
    case 6984:
      fpr (stream, "Sign extend half-word");
      break;
    case 83:
      fpr (stream, "Swap byte order");
      break;
    default:
      {
        /* Decompose the control word.  */
        const int s = ctrl & (1 << 12);
        const int f3 = ctrl & (1 << 11);
        const int i3 = (ctrl >> 9) & 3;
        const int f2 = ctrl & (1 << 8);
        const int i2 = (ctrl >> 6) & 3;
        const int f1 = ctrl & (1 << 5);
        const int i1 = (ctrl >> 3) & 3;
        const int f0 = ctrl & (1 << 2);
        const int i0 = ctrl & 3;

        /* Is this an AND mask?  */
        if ((!s) && (i3 == 3 || f3) && (i2 == 2 || f2)
                 && (i1 == 1 || f1) && (i0 == 0 || f0))
          {
            /* Print the AND mask.  */
            fpr (stream, "and 0x%s%s%s%s",
                 f3 ? "00" : "ff", f2 ? "00" : "ff",
                 f1 ? "00" : "ff", f0 ? "00" : "ff");
          }
        else
          {
            /* Print a human readable version of the control word.  */
            fpr (stream, "%s[%d]:%s[%d]:%s[%d]:%s[%d]",
                 f3 ? (s ? "s" : "z") : "c", i3,
                 f2 ? (s ? "s" : "z") : "c", i2,
                 f1 ? (s ? "s" : "z") : "c", i1,
                 f0 ? (s ? "s" : "z") : "c", i0);
          }
      }
    }
}

static void
_print_bit_field_value (struct disassemble_info *info, const int32_t value)
{
  uint32_t offset = ((uint32_t)value) & 31;
  uint32_t width = (((uint32_t)value) >> 8) & 31;
  if (width != 0)
    info->fprintf_func (info->stream, "<%d:%d>", offset, width);
  else
    info->fprintf_func (info->stream, "%d", offset);
}

static void
_print_bit_field_comment (struct disassemble_info *info, const int32_t value)
{
  uint32_t offset = ((uint32_t)value) & 31;
  uint32_t width = (((uint32_t)value) >> 8) & 31;
  if (width != 0)
    info->fprintf_func (info->stream, "\t; offset=%d width=%d", offset, width);
}

static void
_print_hex_comment (struct disassemble_info *info, const int32_t value)
{
  if (value > 32 || value < -16)
    info->fprintf_func (info->stream, "\t; 0x%x", (unsigned) value);
}

/* A helper function to form a complete 32-bit address for a two-instruction
   sequence, such as ADDPCHI + JL. ADDR is the incomplete address provided
   by the first instruction. REG is the register that holds the incomplete
   address. PC is the address of the second instruction. INFO holds disassble
   info.  */
static uint32_t
_complete_addr_lo (uint32_t addr, int reg, bfd_vma pc, struct disassemble_info *info)
{
  bfd_byte buffer[4];
  uint32_t iword;

  /* Make sure that we don't cross the end of a buffer.  */
  if ((info->buffer_length - (pc - info->buffer_vma) < 4)
      || (info->stop_vma != 0 && pc > (info->stop_vma - 4)))
    return addr;

  /* Read the instruction.  */
  if (info->read_memory_func (pc, buffer, 4, info) != 0)
    return addr;
  iword = (uint32_t) bfd_getl32 (buffer);

  /* Decode the instruction and extract the lower part of the address.  */
  if (((iword & 0xf8000000U) == 0xc0000000U) && (_get_reg1(iword) == reg))
    {
      // J / JL
      return addr + (_get_imm21(iword) << 2);
    }
  else if (((iword & 0xfc000000U) == 0x58000000U) && (_get_reg2(iword) == reg))
    {
      // ADD I15HL
      return addr + _get_imm15(iword, MR32_MODE_I15HL);
    }
  else if (((iword & 0xfc000000U) >= 0x04000000U)
	   && ((iword & 0xfc000000U) <= 0x2c000000U)
	   && (_get_reg2(iword) == reg))
    {
      // LDB/LDUB/LDH/LDUH/LDW/LDEA/STB/STH/STW I15
      return addr + _get_imm15(iword, MR32_MODE_I15_ADDR);
    }

  return addr;
}

int
print_insn_mrisc32 (bfd_vma addr, struct disassemble_info *info)
{
  void *stream = info->stream;
  fprintf_ftype fpr = info->fprintf_func;
  bfd_byte buffer[4];
  uint32_t iword;
  int length;
  int status;
  const mrisc32_opc_info_t *opcode;
  unsigned vec_mode;
  unsigned pack_mode;
  unsigned reg_scale;
  unsigned bit_mode;
  unsigned sel_mode;
  const char *arg1;
  const char *arg2;
  const char *arg3;
  int reg1;
  int reg2;
  int reg3;
  bool is_mov;
  bool is_bit_field;
  int32_t imm_value;
  uint32_t target_addr;

  /* Configure the printer.  */
  info->bytes_per_chunk = 4;

  /* Read the 32-bit instruction word. Since this is a classic 32-bit RISC
     machine, that is all we need.  */
  if ((status = info->read_memory_func (addr, buffer, 4, info)))
    goto fail;
  length = 4;

  iword = (uint32_t) bfd_getl32 (buffer);

  if (iword == 0x00000011u)
    {
      /* The canonical NOP in MRISC32 is OR z, z, z  */
      fpr (stream, "nop");
    }
  else if (iword == 0xc3c00000u)
    {
      /* RET is in fact J lr, #0  */
      fpr (stream, "ret");
    }
  else if ((iword & 0xfc000000u) == 0u && (iword & 0x0000007cu) != 0x7cu)
    {
      /* Type A instruction: REG, REG, REG  */
      opcode = &mrisc32_opc_type_a_info[iword & 0x7f];

      vec_mode = (iword >> 14) & 3;
      pack_mode = 0;
      reg_scale = 0;
      bit_mode = 0;
      sel_mode = 0;
      if (opcode->ft == MR32_FT_PACK)
        pack_mode = (iword >> 7) & 3;
      else if (opcode->ft == MR32_FT_SCALE)
        reg_scale = (iword >> 7) & 3;
      else if (opcode->ft == MR32_FT_BIT)
        bit_mode = (iword >> 7) & 3;
      else if (opcode->ft == MR32_FT_SEL)
        sel_mode = (iword >> 7) & 3;
      reg1 = _get_reg1 (iword);
      reg2 = _get_reg2 (iword);
      reg3 = _get_reg3 (iword);

      /* The canonical MOV in MRISC32 is OR rd, z, rs  */
      is_mov = (opcode->op == 0x11 && reg2 == 0 && pack_mode == 0);

      if (is_mov)
        _print_full_mnemonic (info, "mov", "");
      else
        _print_full_mnemonic (info, opcode->name,
                              _get_suffix (vec_mode, pack_mode, bit_mode, sel_mode));
      arg1 = _is_scalar (opcode, vec_mode, 1) ? scalar_regs[reg1]
                                              : vector_regs[reg1];
      arg2 = _is_scalar (opcode, vec_mode, 2) ? scalar_regs[reg2]
                                              : vector_regs[reg2];
      arg3 = _is_scalar (opcode, vec_mode, 3) ? scalar_regs[reg3]
                                              : vector_regs[reg3];
      if (is_mov)
        fpr (stream, "%s, %s", arg1, arg3);
      else if (_should_swap_operands_2_and_3 (opcode))
        fpr (stream, "%s, %s, %s", arg1, arg3, arg2);
      else if (_has_address_operands (opcode))
        fpr (stream, "%s, [%s, %s", arg1, arg2, arg3);
      else
        fpr (stream, "%s, %s, %s", arg1, arg2, arg3);

      if (reg_scale != 0)
        fpr (stream, "*%d", 1 << reg_scale);
      if (_has_address_operands (opcode))
        fpr (stream, "]");
    }
  else if ((iword & 0xfc000000u) == 0u && (iword & 0x0000007cu) == 0x7cu)
    {
      /* Type B instruction: REG, REG  */
      opcode = &mrisc32_opc_type_b_info[iword & 0x03][_get_func (iword)];

      vec_mode = (iword >> 14) & 2;
      pack_mode = (iword >> 7) & 3;
      reg1 = _get_reg1 (iword);
      reg2 = _get_reg2 (iword);
      _print_full_mnemonic (info, opcode->name,
                            _get_suffix (vec_mode, pack_mode, 0, 0));
      arg1 = _is_scalar (opcode, vec_mode, 1) ? scalar_regs[reg1]
                                              : vector_regs[reg1];
      arg2 = _is_scalar (opcode, vec_mode, 2) ? scalar_regs[reg2]
                                              : vector_regs[reg2];
      fpr (stream, "%s, %s", arg1, arg2);
    }
  else if ((iword & 0xfc000000u) < 0xc0000000u)
    {
      /* Type C instruction: REG, REG, IMM15  */
      /* TODO(m): Handle memory addresses (e.g. PC-relative load/store).  */
      opcode = &mrisc32_opc_type_c_info[(iword >> 26) & 63];
      vec_mode = (iword >> 14) & 2;
      reg1 = _get_reg1 (iword);
      reg2 = _get_reg2 (iword);
      imm_value = _get_imm15 (iword, opcode->mode);

      /* The canonical MOV in MRISC32 is OR rd, z, #imm  */
      is_mov = (opcode->op == 0x11 && reg2 == 0);

      /* Bit field operations use a special immediate syntax.  */
      is_bit_field = ((opcode->op >= 0x13 && opcode->op <= 0x15)
		      || opcode->op == 0x2f);

      if (is_mov)
	_print_full_mnemonic (info, "mov", _get_suffix (vec_mode, 0, 0, 0));
      else if (opcode->op == 0x13 && (imm_value & ~0x1fU) == 0)
        /* ASR is an alias for EBF rd, ra, #N (where N<=31)  */
	_print_full_mnemonic (info, "asr", _get_suffix (vec_mode, 0, 0, 0));
      else if (opcode->op == 0x14 && (imm_value & ~0x1fU) == 0)
        /* LSR is an alias for EBFU rd, ra, #N (where N<=31)  */
	_print_full_mnemonic (info, "lsr", _get_suffix (vec_mode, 0, 0, 0));
      else if (opcode->op == 0x15 && (imm_value & ~0x1fU) == 0)
        /* LSL is an alias for MKBF rd, ra, #N (where N<=31)  */
	_print_full_mnemonic (info, "lsl", _get_suffix (vec_mode, 0, 0, 0));
      else
	_print_full_mnemonic (info, opcode->name, _get_suffix (vec_mode, 0, 0, 0));
      arg1 = _is_scalar (opcode, vec_mode, 1) ? scalar_regs[reg1]
                                              : vector_regs[reg1];
      arg2 = _is_scalar (opcode, vec_mode, 2) ? scalar_regs[reg2]
                                              : vector_regs[reg2];
      if (is_mov)
	fpr (stream, "%s, #%d", arg1, imm_value);
      else if (is_bit_field)
	{
	  fpr (stream, "%s, %s, #", arg1, arg2);
	  _print_bit_field_value (info, imm_value);
	}
      else if (_should_swap_operands_2_and_3 (opcode))
	fpr (stream, "%s, #%d, %s", arg1, imm_value, arg2);
      else if (_has_address_operands (opcode))
	fpr (stream, "%s, [%s, #%d]", arg1, arg2, imm_value);
      else
	fpr (stream, "%s, %s, #%d", arg1, arg2, imm_value);

      /* For SHUF and bit field instructions we add a comment that describes
         the control word.  */
      if (opcode->op == 0x22)
	_print_shuf_comment (info, imm_value);
      else if (is_bit_field)
	_print_bit_field_comment (info, imm_value);
      else
	_print_hex_comment (info, imm_value);
    }
  else if ((iword & 0xfc000000u) < 0xdc000000u)
    {
      /* Type D instruction: REG, IMM21  */
      opcode = &mrisc32_opc_type_d_info[(iword >> 26) & 7];
      enum mr32_mode mode = opcode->mode;

      reg1 = _get_reg1 (iword);
      if (strcmp (opcode->name, "j") == 0 && reg1 == 31)
	{
	  /* B is in fact J pc, #...  */
	  _print_full_mnemonic (info, "b", "");
	  imm_value = _get_imm21 (iword) * 4;
	  fpr (stream, "#(0x");
	  info->print_address_func ((bfd_vma) (addr + imm_value), info);
	  fpr (stream, ")@pc");
	}
      else if (strcmp (opcode->name, "jl") == 0 && reg1 == 31)
	{
	  /* BL is in fact JL pc, #...  */
	  _print_full_mnemonic (info, "bl", "");
	  imm_value = _get_imm21 (iword) * 4;
	  fpr (stream, "#(0x");
	  info->print_address_func ((bfd_vma) (addr + imm_value), info);
	  fpr (stream, ")@pc");
	}
      else
	{
	  _print_full_mnemonic (info, opcode->name, "");
	  arg1 = scalar_regs[reg1];
	  switch (mode)
	    {
	    case MR32_MODE_REGREL21x4:
	      imm_value = _get_imm21 (iword) * 4;
	      if (reg1 == 0)
		{
		  fpr (stream, "%s, #0x", arg1);
		  info->print_address_func ((bfd_vma) imm_value, info);
		}
	      else if (reg1 == 31)
		{
		  fpr (stream, "%s, #(0x", arg1);
		  info->print_address_func ((bfd_vma) (addr + imm_value), info);
		  fpr (stream, ")@pc");
		}
	      else
		{
		  fpr (stream, "%s, #%d", arg1, imm_value);
		  _print_hex_comment (info, imm_value);
		}
	      break;
	    case MR32_MODE_PCREL21x4:
	      imm_value = _get_imm21 (iword) * 4;
	      fpr (stream, "%s, #(0x", arg1);
	      info->print_address_func ((bfd_vma) (addr + imm_value), info);
	      fpr (stream, ")@pc");
	      break;
	    case MR32_MODE_PCREL21H:
	      imm_value = _get_imm21h (iword);
	      fpr (stream, "%s, #(0x", arg1);
	      target_addr = addr + imm_value;
	      target_addr = _complete_addr_lo (target_addr, reg1, addr + 4, info);
	      info->print_address_func ((bfd_vma) target_addr, info);
	      fpr (stream, ")@pchi");
	      break;
	    case MR32_MODE_I21HL:
	      imm_value = _get_imm21hl (iword);
	      fpr (stream, "%s, #%d", arg1, imm_value);
	      _print_hex_comment (info, imm_value);
	      break;
	    default:
	      /* TODO(m): This is an error.  */
	      break;
	    }
	}
    }
  else
    {
      /* Type E instruction: REG, IMM18  */
      opcode = &mrisc32_opc_type_e_info[(iword >> 18) & 7];
      enum mr32_mode mode = opcode->mode;

      _print_full_mnemonic (info, opcode->name, _get_suffix (0, 0, 0, 0));
      arg1 = scalar_regs[_get_reg1 (iword)];
      switch (mode)
	{
	case MR32_MODE_PCREL18x4:
	  fpr (stream, "%s, #0x", arg1);
	  info->print_address_func ((bfd_vma) (addr + _get_imm18 (iword) * 4),
				    info);
	  break;
	default:
	  /* TODO(m): This is an error.  */
	  break;
	}
    }

  return length;

fail:
  info->memory_error_func (status, addr, info);
  return -1;
}
