/* Definitions for MRISC32 opcodes.
   Copyright (C) 2018 Free Software Foundation, Inc.
   Contributed by Marcus Geelnard (m@bitsnbites.eu).

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
   along with this file; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "opcode/mrisc32.h"

const mrisc32_opc_info_t mrisc32_opc_type_a_info[124] =
{
  { "cpuid",    MR32_VC_VVS, MR32_VC_VVV, FALSE, 0x00, 0, MR32_FMT_A, MR32_MODE_3REG },

  /* Load/store */
  { "ldb",      MR32_VC_VSS, MR32_VC_VSV, FALSE, 0x01, 0, MR32_FMT_A, MR32_MODE_3REG_SCALE },
  { "ldh",      MR32_VC_VSS, MR32_VC_VSV, FALSE, 0x02, 0, MR32_FMT_A, MR32_MODE_3REG_SCALE },
  { "ldw",      MR32_VC_VSS, MR32_VC_VSV, FALSE, 0x03, 0, MR32_FMT_A, MR32_MODE_3REG_SCALE },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x04, 0, 0, 0 },
  { "ldub",     MR32_VC_VSS, MR32_VC_VSV, FALSE, 0x05, 0, MR32_FMT_A, MR32_MODE_3REG_SCALE },
  { "lduh",     MR32_VC_VSS, MR32_VC_VSV, FALSE, 0x06, 0, MR32_FMT_A, MR32_MODE_3REG_SCALE },
  { "ldea",     MR32_VC_VSS, MR32_VC_VSV, FALSE, 0x07, 0, MR32_FMT_A, MR32_MODE_3REG_SCALE },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x08, 0, 0, 0 },
  { "stb",      MR32_VC_VSS, MR32_VC_VSV, FALSE, 0x09, 0, MR32_FMT_A, MR32_MODE_3REG_SCALE },
  { "sth",      MR32_VC_VSS, MR32_VC_VSV, FALSE, 0x0a, 0, MR32_FMT_A, MR32_MODE_3REG_SCALE },
  { "stw",      MR32_VC_VSS, MR32_VC_VSV, FALSE, 0x0b, 0, MR32_FMT_A, MR32_MODE_3REG_SCALE },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0c, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0d, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0e, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0f, 0, 0, 0 },

  /* ALU */
  { "or",       MR32_VC_VVS, MR32_VC_VVV, FALSE, 0x10, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "nor",      MR32_VC_VVS, MR32_VC_VVV, FALSE, 0x11, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "and",      MR32_VC_VVS, MR32_VC_VVV, FALSE, 0x12, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "bic",      MR32_VC_VVS, MR32_VC_VVV, FALSE, 0x13, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "xor",      MR32_VC_VVS, MR32_VC_VVV, FALSE, 0x14, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "add",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x15, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "sub",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x16, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "seq",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x17, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "sne",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x18, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "slt",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x19, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "sltu",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x1a, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "sle",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x1b, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "sleu",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x1c, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "min",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x1d, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "max",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x1e, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "minu",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x1f, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "maxu",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x20, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "asr",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x21, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "lsl",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x22, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "lsr",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x23, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "shuf",     MR32_VC_VVS, MR32_VC_VVV, FALSE, 0x24, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x25, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x26, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x27, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x28, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x29, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x2a, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x2b, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x2c, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x2d, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x2e, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x2f, 0, 0, 0 },

  /* Byte/half-word packing.  */
  { "pack",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x30, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "packs",    MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x31, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "packsu",   MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x32, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x33, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x34, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x35, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x36, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x37, 0, 0, 0 },

  /* DSP style saturating and halving arithmetic.  */
  { "adds",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x38, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "addsu",    MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x39, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "addh",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x3a, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "addhu",    MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x3b, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "subs",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x3c, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "subsu",    MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x3d, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "subh",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x3e, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "subhu",    MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x3f, 0, MR32_FMT_A, MR32_MODE_3REG },

  /* Multiplication operations.  */
  { "mulq",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x40, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "mul",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x41, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "mulhi",    MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x42, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "mulhiu",   MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x43, 0, MR32_FMT_A, MR32_MODE_3REG },

  /* Division operations.  */
  { "div",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x44, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "divu",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x45, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "rem",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x46, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "remu",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x47, 0, MR32_FMT_A, MR32_MODE_3REG },

  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x48, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x49, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x4a, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x4b, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x4c, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x4d, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x4e, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x4f, 0, 0, 0 },

  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x50, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x51, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x52, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x53, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x54, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x55, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x56, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x57, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x58, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x59, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x5a, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x5b, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x5c, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x5d, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x5e, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x5f, 0, 0, 0 },

  /* FP arithmetic.  */
  { "fmin",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x60, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "fmax",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x61, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "fseq",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x62, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "fsne",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x63, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "fslt",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x64, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "fsle",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x65, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "fsnan",    MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x66, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x67, 0, 0, 0 },
  { "itof",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x68, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "utof",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x69, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "ftoi",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x6a, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "ftou",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x6b, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "ftoir",    MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x6c, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "ftour",    MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x6d, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "fpack",    MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x6e, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x6f, 0, 0, 0 },
  { "fadd",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x70, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "fsub",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x71, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "fmul",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x72, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "fdiv",     MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x73, 0, MR32_FMT_A, MR32_MODE_3REG },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x74, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x75, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x76, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x77, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x78, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x79, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7a, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7b, 0, 0, 0 },
};

/* Two-operand operations.  */
const mrisc32_opc_info_t mrisc32_opc_type_b_info[4][64] = {
  /* 0x7c  */
  {
    { "clz",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x7c, 0, MR32_FMT_B, MR32_MODE_2REG },
    { "rev",      MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x7c, 1, MR32_FMT_B, MR32_MODE_2REG },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 2, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 3, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 4, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 5, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 6, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 7, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 8, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 9, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 10, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 11, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 12, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 13, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 14, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 15, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 16, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 17, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 18, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 19, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 20, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 21, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 22, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 23, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 24, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 25, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 26, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 27, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 28, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 29, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 30, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 31, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 32, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 33, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 34, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 35, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 36, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 37, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 38, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 39, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 40, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 41, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 42, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 43, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 44, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 45, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 46, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 47, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 48, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 49, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 50, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 51, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 52, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 53, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 54, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 55, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 56, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 57, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 58, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 59, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 60, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 61, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 62, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7c, 63, 0, 0 },
  },

  /* 0x7d  */
  {
    { "fsqrt",    MR32_VC_VVS, MR32_VC_VVV, TRUE,  0x7d, 0, MR32_FMT_B, MR32_MODE_2REG },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 1, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 2, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 3, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 4, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 5, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 6, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 7, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 8, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 9, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 10, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 11, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 12, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 13, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 14, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 15, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 16, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 17, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 18, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 19, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 20, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 21, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 22, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 23, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 24, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 25, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 26, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 27, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 28, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 29, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 30, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 31, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 32, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 33, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 34, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 35, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 36, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 37, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 38, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 39, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 40, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 41, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 42, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 43, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 44, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 45, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 46, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 47, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 48, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 49, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 50, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 51, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 52, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 53, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 54, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 55, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 56, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 57, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 58, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 59, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 60, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 61, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 62, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7d, 63, 0, 0 },
  },

  /* 0x7e  */
  {
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 0, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 1, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 2, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 3, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 4, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 5, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 6, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 7, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 8, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 9, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 10, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 11, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 12, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 13, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 14, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 15, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 16, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 17, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 18, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 19, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 20, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 21, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 22, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 23, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 24, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 25, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 26, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 27, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 28, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 29, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 30, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 31, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 32, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 33, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 34, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 35, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 36, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 37, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 38, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 39, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 40, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 41, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 42, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 43, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 44, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 45, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 46, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 47, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 48, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 49, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 50, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 51, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 52, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 53, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 54, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 55, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 56, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 57, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 58, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 59, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 60, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 61, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 62, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7e, 63, 0, 0 },
  },

  /* 0x7f  */
  {
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 0, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 1, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 2, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 3, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 4, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 5, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 6, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 7, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 8, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 9, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 10, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 11, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 12, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 13, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 14, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 15, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 16, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 17, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 18, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 19, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 20, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 21, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 22, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 23, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 24, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 25, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 26, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 27, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 28, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 29, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 30, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 31, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 32, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 33, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 34, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 35, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 36, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 37, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 38, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 39, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 40, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 41, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 42, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 43, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 44, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 45, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 46, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 47, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 48, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 49, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 50, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 51, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 52, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 53, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 54, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 55, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 56, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 57, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 58, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 59, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 60, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 61, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 62, 0, 0 },
    { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x7f, 63, 0, 0 },
  },
};

const mrisc32_opc_info_t mrisc32_opc_type_c_info[48] =
{
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x00, 0, 0, 0 },

  /* Load/store */
  { "ldb",      MR32_VC_VSS, MR32_VC_NA,  FALSE, 0x01, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "ldh",      MR32_VC_VSS, MR32_VC_NA,  FALSE, 0x02, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "ldw",      MR32_VC_VSS, MR32_VC_NA,  FALSE, 0x03, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x04, 0, 0, 0 },
  { "ldub",     MR32_VC_VSS, MR32_VC_NA,  FALSE, 0x05, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "lduh",     MR32_VC_VSS, MR32_VC_NA,  FALSE, 0x06, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "ldea",     MR32_VC_VSS, MR32_VC_NA,  FALSE, 0x07, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x08, 0, 0, 0 },
  { "stb",      MR32_VC_VSS, MR32_VC_NA,  FALSE, 0x09, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "sth",      MR32_VC_VSS, MR32_VC_NA,  FALSE, 0x0a, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "stw",      MR32_VC_VSS, MR32_VC_NA,  FALSE, 0x0b, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0c, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0d, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0e, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0f, 0, 0, 0 },

  /* ALU */
  { "or",       MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x10, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "nor",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x11, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "and",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x12, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "bic",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x13, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "xor",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x14, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "add",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x15, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "sub",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x16, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "seq",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x17, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "sne",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x18, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "slt",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x19, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "sltu",     MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x1a, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "sle",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x1b, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "sleu",     MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x1c, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "min",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x1d, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "max",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x1e, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "minu",     MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x1f, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "maxu",     MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x20, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "asr",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x21, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "lsl",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x22, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "lsr",      MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x23, 0, MR32_FMT_C, MR32_MODE_IMM15 },
  { "shuf",     MR32_VC_VVS, MR32_VC_NA,  FALSE, 0x24, 0, MR32_FMT_C, MR32_MODE_IMM15 },

  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x25, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x26, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x27, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x28, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x29, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x2a, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x2b, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x2c, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x2d, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x2e, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x2f, 0, 0, 0 },
};

const mrisc32_opc_info_t mrisc32_opc_type_d_info[16] =
{
  /* Conditional branches.  */
  { "bz",       MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x00, 0, MR32_FMT_D, MR32_MODE_PCREL21x4 },
  { "bnz",      MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x01, 0, MR32_FMT_D, MR32_MODE_PCREL21x4 },
  { "bs",       MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x02, 0, MR32_FMT_D, MR32_MODE_PCREL21x4 },
  { "bns",      MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x03, 0, MR32_FMT_D, MR32_MODE_PCREL21x4 },
  { "blt",      MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x04, 0, MR32_FMT_D, MR32_MODE_PCREL21x4 },
  { "bge",      MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x05, 0, MR32_FMT_D, MR32_MODE_PCREL21x4 },
  { "ble",      MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x06, 0, MR32_FMT_D, MR32_MODE_PCREL21x4 },
  { "bgt",      MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x07, 0, MR32_FMT_D, MR32_MODE_PCREL21x4 },

  /* Unconditional branches.  */
  { "j",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x08, 0, MR32_FMT_D, MR32_MODE_REGREL21x4 },
  { "jl",       MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x09, 0, MR32_FMT_D, MR32_MODE_REGREL21x4 },

  /* Load immediate.  */
  { "ldli",     MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0a, 0, MR32_FMT_D, MR32_MODE_IMM21 },
  { "ldhi",     MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0b, 0, MR32_FMT_D, MR32_MODE_IMM21HI },
  { "ldhio",    MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0c, 0, MR32_FMT_D, MR32_MODE_IMM21HIO },

  /* PC hi.  */
  { "addpchi",  MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0d, 0, MR32_FMT_D, MR32_MODE_IMM21HI },

  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0e, 0, 0, 0 },
  { "?",        MR32_VC_NA,  MR32_VC_NA,  FALSE, 0x0f, 0, 0, 0 },
};
