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
_get_imm15 (const uint32_t iw)
{
  return (int32_t) ((iw & 0x00007fff) | ((iw & 0x00004000) ? 0xffff8000 : 0));
}

static int32_t
_get_imm21 (const uint32_t iw)
{
  return (int32_t) ((iw & 0x001fffff) | ((iw & 0x00100000) ? 0xffe00000 : 0));
}

static int32_t
_get_imm21hi (const uint32_t iw)
{
  return _get_imm21 (iw) << 11;
}

static int32_t
_get_imm21hio (const uint32_t iw)
{
  return _get_imm21hi (iw) | 0x000007ff;
}

/* Get opcode suffix.  */
static const char *
_get_suffix (const unsigned vec_mode, const unsigned pack_mode)
{
  if (vec_mode == 1)
    {
      /* Folding.  */
      switch (pack_mode)
        {
        case 0:
          return "/f";
        case 1:
          return ".b/f";
        case 2:
          return ".h/f";
        default:
          return ".?/f";
        }
    }
  else
    {
      /* Non-folding (regular).  */
      switch (pack_mode)
        {
        case 0:
          return "";
        case 1:
          return ".b";
        case 2:
          return ".h";
        default:
          return ".?";
        }
    }
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
  const char *arg1;
  const char *arg2;
  const char *arg3;
  int reg1;
  int reg2;
  int reg3;

  /* Configure the printer.  */
  info->bytes_per_chunk = 4;

  /* Read the 32-bit instruction word. Since this is a classic 32-bit RISC
     machine, that is all we need.  */
  if ((status = info->read_memory_func (addr, buffer, 4, info)))
    goto fail;
  length = 4;

  iword = (uint32_t) bfd_getl32 (buffer);

  if ((iword & 0xfc000000u) == 0u && (iword & 0x0000007cu) != 0x7cu)
    {
      /* Type A instruction: REG, REG, REG  */
      if (iword == 0x00000000u)
        {
          /* The canonical NOP in MRISC32 is CPUID z, z, z  */
          fpr (stream, "nop");
        }
      else
        {
          opcode = &mrisc32_opc_type_a_info[iword & 0x7f];

          vec_mode = (iword >> 14) & 3;
          pack_mode = 0;
          reg_scale = 0;
          if (opcode->mode == MR32_MODE_3REG_SCALE)
            reg_scale = (iword >> 7) & 3;
          else
            pack_mode = (iword >> 7) & 3;
          reg1 = _get_reg1 (iword);
          reg2 = _get_reg2 (iword);
          reg3 = _get_reg3 (iword);
          _print_full_mnemonic (info, opcode->name,
                                _get_suffix (vec_mode, pack_mode));
          arg1 = _is_scalar (opcode, vec_mode, 1) ? scalar_regs[reg1]
                                                  : vector_regs[reg1];
          arg2 = _is_scalar (opcode, vec_mode, 2) ? scalar_regs[reg2]
                                                  : vector_regs[reg2];
          arg3 = _is_scalar (opcode, vec_mode, 3) ? scalar_regs[reg3]
                                                  : vector_regs[reg3];
          if (_should_swap_operands_2_and_3 (opcode))
            fpr (stream, "%s, %s, %s", arg1, arg3, arg2);
          else
            fpr (stream, "%s, %s, %s", arg1, arg2, arg3);

          if (reg_scale != 0)
            fpr (stream, "*%d", 1 << reg_scale);
        }
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
                            _get_suffix (vec_mode, pack_mode));
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
      _print_full_mnemonic (info, opcode->name, _get_suffix (vec_mode, 0));
      arg1 = _is_scalar (opcode, vec_mode, 1) ? scalar_regs[reg1]
                                              : vector_regs[reg1];
      arg2 = _is_scalar (opcode, vec_mode, 2) ? scalar_regs[reg2]
                                              : vector_regs[reg2];
      if (reg2 == 31 && vec_mode == 0)
        {
          if (_should_swap_operands_2_and_3 (opcode))
            {
              fpr (stream, "%s, #(0x", arg1);
              info->print_address_func ((bfd_vma) (addr + _get_imm15 (iword)),
                                        info);
              fpr (stream, ")@pc, %s", arg2);
            }
          else
            {
              fpr (stream, "%s, %s, #(0x", arg1, arg2);
              info->print_address_func ((bfd_vma) (addr + _get_imm15 (iword)),
                                        info);
              fpr (stream, ")@pc");
            }
        }
      else
        {
          if (_should_swap_operands_2_and_3 (opcode))
            fpr (stream, "%s, #%d, %s", arg1, _get_imm15 (iword), arg2);
          else
            fpr (stream, "%s, %s, #%d", arg1, arg2, _get_imm15 (iword));
        }
    }
  else
    {
      /* Type D instruction: REG, IMM21  */
      opcode = &mrisc32_opc_type_d_info[(iword >> 26) & 15];
      _print_full_mnemonic (info, opcode->name, _get_suffix (0, 0));
      arg1 = scalar_regs[_get_reg1 (iword)];
      switch (opcode->mode)
        {
        case MR32_MODE_PCREL21x4:
          fpr (stream, "%s, #0x", arg1);
          info->print_address_func ((bfd_vma) (addr + _get_imm21 (iword) * 4),
                                    info);
          break;
        case MR32_MODE_REGREL21x4:
          if (_get_reg1 (iword) == 31)
            {
              fpr (stream, "%s, #0x", arg1);
              info->print_address_func (
                  (bfd_vma) (addr + _get_imm21 (iword) * 4), info);
            }
          else
            fpr (stream, "%s, #%d", arg1, _get_imm21 (iword) * 4);
          break;
        case MR32_MODE_IMM21:
          fpr (stream, "%s, #%d", arg1, _get_imm21 (iword));
          break;
        case MR32_MODE_IMM21HI:
          fpr (stream, "%s, #0x%08x", arg1, _get_imm21hi (iword));
          break;
        case MR32_MODE_IMM21HIO:
          fpr (stream, "%s, #0x%08x", arg1, _get_imm21hio (iword));
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
