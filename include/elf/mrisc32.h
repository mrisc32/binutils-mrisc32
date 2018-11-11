/* MRISC32 elf support for BFD.
   Copyright (C) 2018 Free Software Foundation, Inc.
   Contributed by Marcus Geelnard (m@bitsnbites.eu).

   This file is part of BFD, the Binary File Descriptor library.

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
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#ifndef _ELF_MRISC32_H
#define _ELF_MRISC32_H

#include "elf/reloc-macros.h"

/* Relocations.  */
START_RELOC_NUMBERS(elf_mrisc32_reloc_type)
  /* Relocation types used by the dynamic linker.  */
  RELOC_NUMBER (R_MRISC32_NONE,    0)
  RELOC_NUMBER (R_MRISC32_32,      1)

  /* Relocation types not used by the dynamic linker.  */
  RELOC_NUMBER (R_MRISC32_PCREL_18X4, 2)
  RELOC_NUMBER (R_MRISC32_PCREL_21X4, 3)
  RELOC_NUMBER (R_MRISC32_PCREL_HI20, 4)
  RELOC_NUMBER (R_MRISC32_PCREL_LO12, 5)
  RELOC_NUMBER (R_MRISC32_PCREL_LO10X4, 6)
  RELOC_NUMBER (R_MRISC32_I14, 7)
  RELOC_NUMBER (R_MRISC32_IHI14, 8)
  RELOC_NUMBER (R_MRISC32_I15, 9)
  RELOC_NUMBER (R_MRISC32_I21, 10)
  RELOC_NUMBER (R_MRISC32_IHI21, 11)
  RELOC_NUMBER (R_MRISC32_IHI20, 12)
  RELOC_NUMBER (R_MRISC32_ILO12, 13)
  RELOC_NUMBER (R_MRISC32_ILO10X4, 14)
  RELOC_NUMBER (R_MRISC32_I21X4, 15)
END_RELOC_NUMBERS(R_MRISC32_max)

#endif /* _ELF_MRISC32_H */
