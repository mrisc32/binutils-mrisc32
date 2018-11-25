# Linker script for MRISC32 systems.
# Marcus Geelnard <m@bitsnbites.eu>
#
# Copyright (C) 2020 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

# These variables may be overridden by the emulation file.  The
# defaults are appropriate for an MRISC32 machine that loads all
# sections to RAM (starting at 0x00000000) before system reset (e.g.
# a simulator).
test -z "$ARCH" && ARCH=mrisc32
test -z "$ENTRY" && ENTRY=_start
test -z "$TEXT_START_ADDR" && TEXT_START_ADDR=0x00000200
test -z "$DATA_START_ADDR" && DATA_START_ADDR=.
test -z "$SECTION_ALIGN" && SECTION_ALIGN=32

cat <<EOF
/* Copyright (C) 2020 Free Software Foundation, Inc.

   Copying and distribution of this script, with or without modification,
   are permitted in any medium without royalty provided the copyright
   notice and this notice are preserved.  */

OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})
${RELOCATING+ENTRY(${ENTRY})}
${RELOCATING+${LIB_SEARCH_DIRS}}
${RELOCATING+${INPUT_FILES}}

SECTIONS
{
  /* Read-only sections first.  */
  ${RELOCATING+. = ${TEXT_START_ADDR};}
  .text :
  {
    *(.text.entry)
    *(.text.start)
    *(.text*)
  }
  ${RELOCATING+. = ALIGN(${SECTION_ALIGN});}

  .rodata :
  {
    *(.rodata*)
  }
  ${RELOCATING+. = ALIGN(${SECTION_ALIGN});}

  .eh_frame ${RELOCATING-0} :
  {
    KEEP (*(.eh_frame))${RELOCATING+*(.eh_frame.*)}
  }
  ${RELOCATING+. = ALIGN(${SECTION_ALIGN});}

  /* Read/write sections.  */
  ${RELOCATING+. = ${DATA_START_ADDR};}
  ${RELOCATING+. = ALIGN(${SECTION_ALIGN});}
  ${RELOCATING+__data_start = .;}
  .data :
  {
    *(.data*)
  }
  ${RELOCATING+__data_end = .;}
  ${RELOCATING+. = ALIGN(${SECTION_ALIGN});}

  ${RELOCATING+__bss_start = .;}
  .bss (NOLOAD) :
  {
    *(.bss*)
    *(COMMON)
  }
  ${RELOCATING+. = ALIGN(${SECTION_ALIGN});}
  ${RELOCATING+__bss_end = .;}

  /* End of all section: This is normally where the heap starts.  */
  ${RELOCATING+_end = .;}
}
EOF
