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
  { "z", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "s12", "s13", "s14", "s15",
    "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23",
    "s24", "s25", "fp", "tp", "sp", "vl", "lr", "pc" };

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
_get_imm14 (const uint32_t iw)
{
  const uint32_t imm = iw & 0x00003fff;
  if (iw & 0x00004000U)
    return (int32_t) ((imm << 18) | ((imm & 1) ? 0x0003ffff : 0));
  else
    return (int32_t) (imm | ((imm & 0x00002000U) ? 0xffffc000 : 0));
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
_get_imm21hi (const uint32_t iw)
{
  return (int32_t) ((iw << 11) | ((iw & 1) ? 0x7ff : 0));
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
    {"", "_pn", "_np", "_nn"},	       /* Non-folding.  */
    {"/f", "_pn/f", "_np/f", "_nn/f"}  /* Folding.  */
  };

  static const char* SEL_SUFFIX[2][4] = {
    {"", "_1", "_2", "_3"},	    /* Non-folding.  */
    {"/f", "_1/f", "_2/f", "_3/f"}  /* Folding.  */
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

static bfd_boolean
_is_scalar (const mrisc32_opc_info_t *opcode, const unsigned vec_mode,
            const int arg_no)
{
  enum mr32_vector_conf vc;

  if (vec_mode == 0)
    return TRUE;

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
      return TRUE;

    case MR32_VC_VVV:
      return FALSE;

    case MR32_VC_VVS:
      return (arg_no == 3) ? TRUE : FALSE;

    case MR32_VC_VSS:
      return (arg_no != 1) ? TRUE : FALSE;

    case MR32_VC_VSV:
      return (arg_no == 2) ? TRUE : FALSE;
    }
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

static bfd_boolean
_should_swap_operands_2_and_3 (const mrisc32_opc_info_t *opcode)
{
  /* sub a, b, c  ->  sub a, c, b */
  return (opcode->op == 0x16u) ? TRUE : FALSE;
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

        /* Print a human readable version of the control word.  */
        fpr (stream, "%s[%d]:%s[%d]:%s[%d]:%s[%d]",
            f3 ? (s ? "s" : "z") : "c", i3,
            f2 ? (s ? "s" : "z") : "c", i2,
            f1 ? (s ? "s" : "z") : "c", i1,
            f0 ? (s ? "s" : "z") : "c", i0);
      }
    }
}

static void
_print_hex_comment (struct disassemble_info *info, const int32_t value)
{
  if (value > 32 || value < -16)
    info->fprintf_func (info->stream, "\t; 0x%x", (unsigned) value);
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
  bfd_boolean is_mov;
  int32_t imm_value;

  /* Configure the printer.  */
  info->bytes_per_chunk = 4;

  /* Read the 32-bit instruction word. Since this is a classic 32-bit RISC
     machine, that is all we need.  */
  if ((status = info->read_memory_func (addr, buffer, 4, info)))
    goto fail;
  length = 4;

  iword = (uint32_t) bfd_getl32 (buffer);

  if (iword == 0x00000000u)
    {
      /* The canonical NOP in MRISC32 is CPUID z, z, z  */
      fpr (stream, "nop");
    }
  else if (iword == 0xd3c00000u)
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
      is_mov = (opcode->op == 0x10 && reg2 == 0 && pack_mode == 0);

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
      else
        fpr (stream, "%s, %s, %s", arg1, arg2, arg3);

      if (reg_scale != 0)
        fpr (stream, "*%d", 1 << reg_scale);
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
      /* Type C instruction: REG, REG, IMM14  */
      /* TODO(m): Handle memory addresses (e.g. PC-relative load/store).  */
      opcode = &mrisc32_opc_type_c_info[(iword >> 26) & 63];
      vec_mode = (iword >> 14) & 2;
      reg1 = _get_reg1 (iword);
      reg2 = _get_reg2 (iword);

      /* The canonical MOV in MRISC32 is OR rd, z, #imm  */
      is_mov = (opcode->op == 0x10 && reg2 == 0);

      if (is_mov)
	_print_full_mnemonic (info, "mov", _get_suffix (vec_mode, 0, 0, 0));
      else
	_print_full_mnemonic (info, opcode->name, _get_suffix (vec_mode, 0, 0, 0));
      arg1 = _is_scalar (opcode, vec_mode, 1) ? scalar_regs[reg1]
                                              : vector_regs[reg1];
      arg2 = _is_scalar (opcode, vec_mode, 2) ? scalar_regs[reg2]
                                              : vector_regs[reg2];
      imm_value = _get_imm14 (iword);
      if (reg2 == 31 && vec_mode == 0)
	{
	  // TODO(m): Handle swapped operands.
	  fpr (stream, "%s, %s, #(0x", arg1, arg2);
	      info->print_address_func ((bfd_vma) (addr + imm_value),
					info);
	  fpr (stream, ")@pc");
	}
      else
	{
	  if (is_mov)
	    fpr (stream, "%s, #%d", arg1, imm_value);
	  else if (_should_swap_operands_2_and_3 (opcode))
	    fpr (stream, "%s, #%d, %s", arg1, imm_value, arg2);
	  else
	    fpr (stream, "%s, %s, #%d", arg1, arg2, imm_value);
	}

      // For SHUF, we add a comment that describes the control word.
      if (opcode->op == 0x24)
	_print_shuf_comment (info, imm_value);
      else
	_print_hex_comment (info, imm_value);
    }
  else if ((iword & 0xfc000000u) < 0xdc000000u)
    {
      /* Type D instruction: REG, IMM21  */
      opcode = &mrisc32_opc_type_d_info[(iword >> 26) & 7];
      enum mr32_mode mode = opcode->mode;

      _print_full_mnemonic (info, opcode->name, _get_suffix (0, 0, 0, 0));
      arg1 = scalar_regs[_get_reg1 (iword)];
      switch (mode)
	{
	case MR32_MODE_REGREL21x4:
	  imm_value = _get_imm21 (iword) * 4;
	  fpr (stream, "%s, #%d", arg1, imm_value);
	  _print_hex_comment (info, imm_value);
	  break;
	case MR32_MODE_IMM21:
	  imm_value = _get_imm21 (iword);
	  fpr (stream, "%s, #%d", arg1, imm_value);
	  _print_hex_comment (info, imm_value);
	  break;
	case MR32_MODE_IMM21HI:
	  imm_value = _get_imm21hi (iword);
	  fpr (stream, "%s, #%d", arg1, imm_value);
	  _print_hex_comment (info, imm_value);
	  break;
	default:
	  /* TODO(m): This is an error.  */
	  break;
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
