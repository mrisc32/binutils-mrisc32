;------------------------------------------------------
; Code section
;------------------------------------------------------

.text
    .global _start

_start:
    nop
    ldi     s3, #34
    ldhi    s4, #0xffff0000
    ldhio   s5, #0x0123ffff
    or      s1, z, #7-2*4  ; -1

    or      s6, z, #loop+7

    bl      #foo

    ldhi    s7, 0x12345678@hi
    or      s7, s7, 0x12345678@lo

    ldw     s9, pc, #loop@pc
    ldi     s8, #hello_len
    ldhi    s9, #hello@hi
    add     s9, s9, #hello@lo

    ; A small loop.
loop:
    add     s3, s3, s1
    bz      s3, #loop

    ; Some random vector instructions.
    or      v3, vz, s3
    fmul.h  v3, v3, s1
    add.b/f v3, v3, v1
    ldw     v29, s9, #4

    mov     v12, v3
    mov     v13, s1
    mov     v14, #1234

    ; Terminate the program (unconditional jump to PC + 0).
end:
    b       #end

foo:
    seq     s1, s2, #42
    j       lr


;------------------------------------------------------
; Data section
;------------------------------------------------------

.data

stuff:
    .zero    123

hello:
    .asciz    "Hello, world!\n"
    hello_len = . - hello
