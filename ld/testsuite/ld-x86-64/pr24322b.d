#source: pr24322c.s
#source: pr24322b.s
#as: --64 -defsym __64_bit__=1 -mx86-used-note=yes
#ld: -z shstk -melf_x86_64
#readelf: -n

Displaying notes found in: .note.gnu.property
  Owner                 Data size	Description
  GNU                  0x00000030	NT_GNU_PROPERTY_TYPE_0
      Properties: x86 feature: SHSTK
	x86 ISA used: <None>
	x86 feature used: x86
