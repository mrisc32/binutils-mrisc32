/* MRISC32 elf support for BFD.
   Copyright (C) 2018 Free Software Foundation, Inc.
   Contributed by Marcus Geelnard (m@bitsnbites.eu).

   Largely inspired by the moxie target.

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

#include "sysdep.h"
#include "bfd.h"
#include "elf-bfd.h"
#include "libbfd.h"
#include "elf/mrisc32.h"

static reloc_howto_type elf_mrisc32_howto_table_rel[] =
{
  /* No relocation.  */
  HOWTO (R_MRISC32_NONE,	/* type */
         0,			/* rightshift */
         3,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         0,			/* bitsize */
         FALSE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_NONE",	/* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0,			/* dst_mask */
         FALSE),		/* pcrel_offset */

  /* 32 bit relocation.  */
  HOWTO (R_MRISC32_32,		/* type */
         0,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         32,			/* bitsize */
         FALSE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_32",	/* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0xffffffff,		/* dst_mask */
         FALSE),		/* pcrel_offset */

  /* 15 bit PC relative relocation.  */
  HOWTO (R_MRISC32_PCREL_15,	/* type */
         0,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         15,			/* bitsize */
         TRUE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_PCREL_15",	/* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0x00007fff,		/* dst_mask */
         TRUE),			/* pcrel_offset */

  /* 21+2 bit PC relative relocation.  */
  HOWTO (R_MRISC32_PCREL_21X4,	/* type */
         2,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         21,			/* bitsize */
         TRUE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_PCREL_21X4", /* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0x001fffff,		/* dst_mask */
         TRUE),			/* pcrel_offset */

  /* High 21 bits of PC relative relocation.  */
  HOWTO (R_MRISC32_PCREL_HI21,	/* type */
         11,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         21,			/* bitsize */
         TRUE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_PCREL_HI21", /* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0x001fffff,		/* dst_mask */
         TRUE),			/* pcrel_offset */

  /* Low 11 bits of PC relative relocation.  */
  HOWTO (R_MRISC32_PCREL_LO11,	/* type */
         0,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         11,			/* bitsize */
         TRUE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_PCREL_LO11", /* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0x000007ff,		/* dst_mask */
         TRUE),			/* pcrel_offset */

  /* Low 9+2 bits of PC relative relocation.  */
  HOWTO (R_MRISC32_PCREL_LO9X4,	/* type */
         2,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         9,			/* bitsize */
         TRUE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_PCREL_LO9X4", /* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0x000001ff,		/* dst_mask */
         TRUE),			/* pcrel_offset */

  /* 15 bits immediate relocation.  */
  HOWTO (R_MRISC32_I15,		/* type */
         0,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         15,			/* bitsize */
         FALSE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_I15",	/* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0x00007fff,		/* dst_mask */
         FALSE),		/* pcrel_offset */

  /* 21 bits immediate relocation.  */
  HOWTO (R_MRISC32_I21,		/* type */
         0,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         21,			/* bitsize */
         FALSE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_I21",	/* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0x001fffff,		/* dst_mask */
         FALSE),		/* pcrel_offset */

  /* 21 high bits immediate relocation.  */
  HOWTO (R_MRISC32_IHI21,	/* type */
         11,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         21,			/* bitsize */
         FALSE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_IHI21",	/* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0x001fffff,		/* dst_mask */
         FALSE),		/* pcrel_offset */

  /* 21 high bits of a 32-bit value, with one-filled low 11 bits.  */
  HOWTO (R_MRISC32_IHIO21,	/* type */
         11,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         21,			/* bitsize */
         FALSE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_IHIO21",	/* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0x001fffff,		/* dst_mask */
         FALSE),		/* pcrel_offset */

  /* 11 low bits immediate relocation.  */
  HOWTO (R_MRISC32_ILO11,	/* type */
         0,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         11,			/* bitsize */
         FALSE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_ILO11",	/* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0x000007ff,		/* dst_mask */
         FALSE),		/* pcrel_offset */

  /* 9+2 low bits immediate relocation.  */
  HOWTO (R_MRISC32_ILO9X4,	/* type */
         2,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         9,			/* bitsize */
         FALSE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_ILO9X4",	/* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0x000001ff,		/* dst_mask */
         FALSE),		/* pcrel_offset */

  /* 21+2 bits immediate relocation.  */
  HOWTO (R_MRISC32_I21X4,	/* type */
         2,			/* rightshift */
         2,			/* size (0=1, 1=2, 2=4, 3=0, 4=8, 5=3) */
         21,			/* bitsize */
         FALSE,			/* pc_relative */
         0,			/* bitpos */
         complain_overflow_dont, /* complain_on_overflow */
         bfd_elf_generic_reloc, /* special_function */
         "R_MRISC32_I21X4",	/* name */
         FALSE,			/* partial_inplace */
         0,			/* src_mask */
         0x001fffff,		/* dst_mask */
         FALSE),		/* pcrel_offset */
};

/* Map BFD reloc types to MRISC32 ELF reloc types.  */

struct mrisc32_reloc_map
{
  bfd_reloc_code_real_type bfd_reloc_val;
  unsigned int mrisc32_reloc_val;
};

static const struct mrisc32_reloc_map mrisc32_reloc_map [] =
{
  { BFD_RELOC_NONE,                R_MRISC32_NONE },
  { BFD_RELOC_32,                  R_MRISC32_32 },
  { BFD_RELOC_MRISC32_PCREL_15,    R_MRISC32_PCREL_15 },
  { BFD_RELOC_MRISC32_PCREL_21X4,  R_MRISC32_PCREL_21X4 },
  { BFD_RELOC_MRISC32_PCREL_HI21,  R_MRISC32_PCREL_HI21 },
  { BFD_RELOC_MRISC32_PCREL_LO11,  R_MRISC32_PCREL_LO11 },
  { BFD_RELOC_MRISC32_PCREL_LO9X4, R_MRISC32_PCREL_LO9X4 },
  { BFD_RELOC_MRISC32_I15,         R_MRISC32_I15 },
  { BFD_RELOC_MRISC32_I21,         R_MRISC32_I21 },
  { BFD_RELOC_MRISC32_IHI21,       R_MRISC32_IHI21 },
  { BFD_RELOC_MRISC32_IHIO21,      R_MRISC32_IHIO21 },
  { BFD_RELOC_MRISC32_ILO11,       R_MRISC32_ILO11 },
  { BFD_RELOC_MRISC32_ILO9X4,      R_MRISC32_ILO9X4 },
  { BFD_RELOC_MRISC32_I21X4,       R_MRISC32_I21X4 },
};


/* Lookup functions. */

static reloc_howto_type *
mrisc32_reloc_type_lookup (bfd *abfd ATTRIBUTE_UNUSED,
                         bfd_reloc_code_real_type code)
{
  unsigned int i;

  for (i = sizeof (mrisc32_reloc_map) / sizeof (mrisc32_reloc_map[0]);
       i--;)
    if (mrisc32_reloc_map [i].bfd_reloc_val == code)
      return & elf_mrisc32_howto_table_rel [mrisc32_reloc_map[i].mrisc32_reloc_val];

  return NULL;
}

static reloc_howto_type *
mrisc32_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED, const char *r_name)
{
  unsigned int i;

  for (i = 0;
       i < sizeof (elf_mrisc32_howto_table_rel) / sizeof (elf_mrisc32_howto_table_rel[0]);
       i++)
    if (elf_mrisc32_howto_table_rel[i].name != NULL
        && strcasecmp (elf_mrisc32_howto_table_rel[i].name, r_name) == 0)
      return &elf_mrisc32_howto_table_rel[i];

  return NULL;
}


/* Set the howto pointer for an MRISC32 ELF reloc.  */

static bfd_boolean
mrisc32_info_to_howto_rela (bfd *abfd,
			  arelent *cache_ptr,
			  Elf_Internal_Rela *dst)
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  if (r_type >= (unsigned int) R_MRISC32_max)
    {
      /* xgettext:c-format */
      _bfd_error_handler (_("%pB: unsupported relocation type %#x"),
                          abfd, r_type);
      bfd_set_error (bfd_error_bad_value);
      return FALSE;
    }
  cache_ptr->howto = & elf_mrisc32_howto_table_rel [r_type];
  return TRUE;
}

/* Perform a single relocation.  By default we use the standard BFD
   routines, but a few relocs, we have to do them ourselves.  */

static bfd_reloc_status_type
mrisc32_final_link_relocate (reloc_howto_type *howto,
			   bfd *input_bfd,
			   asection *input_section,
			   bfd_byte *contents,
			   Elf_Internal_Rela *rel,
			   bfd_vma relocation)
{
  bfd_reloc_status_type r = bfd_reloc_ok;

  switch (howto->type)
    {
    /* TODO(m): If necessary add support for complex relocations,
       otherwise just use _bfd_final_link_relocate directly instead
       of implementing this wrapper function. */
    default:
      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
                                    contents, rel->r_offset,
                                    relocation, rel->r_addend);
    }

  return r;
}

/* Relocate an MRISC32 ELF section.

   The RELOCATE_SECTION function is called by the new ELF backend linker
   to handle the relocations for a section.

   The relocs are always passed as Rela structures; if the section
   actually uses Rel structures, the r_addend field will always be
   zero.

   This function is responsible for adjusting the section contents as
   necessary, and (if using Rela relocs and generating a relocatable
   output file) adjusting the reloc addend as necessary.

   This function does not have to worry about setting the reloc
   address or the reloc symbol index.

   LOCAL_SYMS is a pointer to the swapped in local symbols.

   LOCAL_SECTIONS is an array giving the section in the input file
   corresponding to the st_shndx field of each local symbol.

   The global hash table entry for the global symbols can be found
   via elf_sym_hashes (input_bfd).

   When generating relocatable output, this function must handle
   STB_LOCAL/STT_SECTION symbols specially.  The output symbol is
   going to be the section symbol corresponding to the output
   section, which means that the addend must be adjusted
   accordingly.  */

static bfd_boolean
mrisc32_elf_relocate_section (bfd *output_bfd,
			    struct bfd_link_info *info,
			    bfd *input_bfd,
			    asection *input_section,
			    bfd_byte *contents,
			    Elf_Internal_Rela *relocs,
			    Elf_Internal_Sym *local_syms,
			    asection **local_sections)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  symtab_hdr = & elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  relend     = relocs + input_section->reloc_count;

  for (rel = relocs; rel < relend; rel ++)
    {
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_reloc_status_type r;
      const char *name;
      int r_type;

      r_type = ELF32_R_TYPE (rel->r_info);
      r_symndx = ELF32_R_SYM (rel->r_info);
      howto  = elf_mrisc32_howto_table_rel + r_type;
      h      = NULL;
      sym    = NULL;
      sec    = NULL;

      if (r_symndx < symtab_hdr->sh_info)
        {
          sym = local_syms + r_symndx;
          sec = local_sections [r_symndx];
          relocation = _bfd_elf_rela_local_sym (output_bfd, sym, &sec, rel);

	  name = bfd_elf_string_from_elf_section
	    (input_bfd, symtab_hdr->sh_link, sym->st_name);
	  name = name == NULL ? bfd_section_name (input_bfd, sec) : name;
	}
      else
        {
          bfd_boolean unresolved_reloc, warned, ignored;

	  RELOC_FOR_GLOBAL_SYMBOL (info, input_bfd, input_section, rel,
				   r_symndx, symtab_hdr, sym_hashes,
				   h, sec, relocation,
				   unresolved_reloc, warned, ignored);

	  name = h->root.root.string;
	}

      if (sec != NULL && discarded_section (sec))
        RELOC_AGAINST_DISCARDED_SECTION (info, input_bfd, input_section,
                                         rel, 1, relend, howto, 0, contents);

      if (bfd_link_relocatable (info))
        continue;

      r = mrisc32_final_link_relocate (howto, input_bfd, input_section,
                                     contents, rel, relocation);

      if (r != bfd_reloc_ok)
        {
          const char * msg = NULL;

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      (*info->callbacks->reloc_overflow)
		(info, (h ? &h->root : NULL), name, howto->name,
		 (bfd_vma) 0, input_bfd, input_section, rel->r_offset);
	      break;

	    case bfd_reloc_undefined:
	      (*info->callbacks->undefined_symbol)
		(info, name, input_bfd, input_section, rel->r_offset, TRUE);
	      break;

	    case bfd_reloc_outofrange:
	      msg = _("internal error: out of range error");
	      break;

	    case bfd_reloc_notsupported:
	      msg = _("internal error: unsupported relocation error");
	      break;

	    case bfd_reloc_dangerous:
	      msg = _("internal error: dangerous relocation");
	      break;

	    default:
	      msg = _("internal error: unknown error");
	      break;
	    }

	  if (msg)
	    (*info->callbacks->warning) (info, msg, name, input_bfd,
					 input_section, rel->r_offset);
	}
    }

  return TRUE;
}

/* Return the section that should be marked against GC for a given
   relocation.  */

static asection *
mrisc32_elf_gc_mark_hook (asection *sec,
			struct bfd_link_info *info,
			Elf_Internal_Rela *rel,
			struct elf_link_hash_entry *h,
			Elf_Internal_Sym *sym)
{
  return _bfd_elf_gc_mark_hook (sec, info, rel, h, sym);
}

/* Look through the relocs for a section during the first phase.
   Since we don't do .gots or .plts, we just need to consider the
   virtual table relocs for gc.  */

static bfd_boolean
mrisc32_elf_check_relocs (bfd *abfd,
			struct bfd_link_info *info,
			asection *sec,
			const Elf_Internal_Rela *relocs)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;

  if (bfd_link_relocatable (info))
    return TRUE;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);

  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      struct elf_link_hash_entry *h;
      unsigned long r_symndx;

      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
        h = NULL;
      else
        {
          h = sym_hashes[r_symndx - symtab_hdr->sh_info];
          while (h->root.type == bfd_link_hash_indirect
                 || h->root.type == bfd_link_hash_warning)
            h = (struct elf_link_hash_entry *) h->root.u.i.link;
        }
    }

  return TRUE;
}


/* Target definitions */

#define ELF_ARCH		bfd_arch_mrisc32
#define ELF_MACHINE_CODE	EM_MRISC32
#define ELF_MAXPAGESIZE		0x1

#define TARGET_LITTLE_SYM	mrisc32_elf32_vec
#define TARGET_LITTLE_NAME	"elf32-mrisc32"

#define elf_info_to_howto_rel			NULL
#define elf_info_to_howto			mrisc32_info_to_howto_rela
#define elf_backend_relocate_section		mrisc32_elf_relocate_section
#define elf_backend_gc_mark_hook		mrisc32_elf_gc_mark_hook
#define elf_backend_check_relocs		mrisc32_elf_check_relocs

#define elf_backend_can_gc_sections		1
#define elf_backend_rela_normal			1

#define bfd_elf32_bfd_reloc_type_lookup		mrisc32_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup		mrisc32_reloc_name_lookup

#include "elf32-target.h"
