/* Definitions for decoding the MRISC32 opcode table.
   Copyright (C) 2018 Free Software Foundation, Inc.
   Contributed by Marcus Geelnard (m@bitsnbites.eu).

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef OPCODE_MRISC32_H
#define OPCODE_MRISC32_H

/* Instruction formats.  */
enum mr32_fmt
{
  MR32_FMT_BAD = 0,
  MR32_FMT_A,
  MR32_FMT_B,
  MR32_FMT_C,
  MR32_FMT_D,
  MR32_FMT_E,
};

/* Instruction modes.  */
enum mr32_mode
{
  MR32_MODE_DEFAULT = 0,
  MR32_MODE_3REG,
  MR32_MODE_3REG_ADDR,
  MR32_MODE_2REG,
  MR32_MODE_I15_ADDR,
  MR32_MODE_I15HL,
  MR32_MODE_I21HL,
  MR32_MODE_REGREL21x4,
  MR32_MODE_PCREL21x4,
  MR32_MODE_PCREL21H,
  MR32_MODE_PCREL18x4,
};

/* Flavor field interpretation.  */
enum mr32_flavor_type
{
  MR32_FT_NONE = 0,
  MR32_FT_PACK,
  MR32_FT_SCALE,
  MR32_FT_BIT,
  MR32_FT_SEL,
};

/* Vector argument configurations.  */
enum mr32_vector_conf
{
  MR32_VC_NA = 0,
  MR32_VC_VVV,
  MR32_VC_VVS,
  MR32_VC_VSS,
  MR32_VC_VSV,
};

typedef struct mrisc32_opc_info_t
{
  const char *name;          /* Opcode name (mnemonic).  */
  enum mr32_vector_conf vc2; /* Vector operands for vector mode 2.  */
  enum mr32_vector_conf vc3; /* Vector operands for vector mode 3.  */
  enum mr32_flavor_type ft;  /* FT field interpretation (packed ops etc).  */
  unsigned char op;          /* Operation code.  */
  unsigned char func;        /* Function code (for 2-operand type B ops).  */
  enum mr32_fmt format;      /* Format type.  */
  enum mr32_mode mode;       /* Operand mode.  */
} mrisc32_opc_info_t;

extern const mrisc32_opc_info_t mrisc32_opc_type_a_info[124];
extern const mrisc32_opc_info_t mrisc32_opc_type_b_info[4][64];
extern const mrisc32_opc_info_t mrisc32_opc_type_c_info[48];
extern const mrisc32_opc_info_t mrisc32_opc_type_d_info[7];
extern const mrisc32_opc_info_t mrisc32_opc_type_e_info[8];

#endif /* OPCODE_MRISC32_H */
