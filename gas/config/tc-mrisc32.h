/* tc-mrisc32.h -- header file for tc-mrisc32.c.
   Copyright (C) 2018 Free Software Foundation, Inc.

   Contributed by Marcus Geelnard (m@bitsnbites.eu).
   Based on the moxie and RISC-V targets.

   This file is part of GAS.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3. If not,
   see <http://www.gnu.org/licenses/>.  */

#ifndef TC_MRISC32
#define TC_MRISC32

#include "opcode/mrisc32.h"

#define TARGET_BYTES_BIG_ENDIAN 0

#define TARGET_ARCH bfd_arch_mrisc32

#define WORKING_DOT_WORD	1
#define LOCAL_LABELS_FB 	1

#define TARGET_FORMAT "elf32-mrisc32"

#define md_undefined_symbol(NAME)           0

/* Add machine specific pseudo ops.  */
#define md_pop_insert()         mrisc32_pop_insert ()
extern void mrisc32_pop_insert (void);

/* These macros must be defined, but is will be a fatal assembler
   error if we ever hit them.  */
#define md_estimate_size_before_relax(A, B) (as_fatal (_("estimate size\n")), 0)
#define md_convert_frag(B, S, F)            as_fatal (_("convert_frag\n"))

/* Call md_pcrel_from_section(), not md_pcrel_from().  */
extern long md_pcrel_from_section (struct fix *, segT);
#define MD_PCREL_FROM_SECTION(FIX, SEC) md_pcrel_from_section (FIX, SEC)

#define md_section_align(SEGMENT, SIZE)     (SIZE)

#endif /* TC_MRISC32 */
