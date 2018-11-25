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
    ${RELOCATING+KEEP (*(SORT_NONE(.init)))
                 KEEP (*(SORT_NONE(.fini)))}
  }
  ${RELOCATING+. = ALIGN(${SECTION_ALIGN});}

  .rodata :
  {
    *(.rodata*)
  }
  ${RELOCATING+. = ALIGN(${SECTION_ALIGN});}

  .ctor :
  {
    __CTOR_START = .;
    /* The compiler uses crtbegin.o to find the start
       of the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */

    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))

    /* We don't want to include the .ctor section from
       the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */

    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __CTOR_END = .;
  }
  ${RELOCATING+. = ALIGN(${SECTION_ALIGN});}

  .dtor :
  {
    __DTOR_START = .;
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    __DTOR_END = .;
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
