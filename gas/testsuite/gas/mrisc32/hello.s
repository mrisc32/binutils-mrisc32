;------------------------------------------------------
; Code section
;------------------------------------------------------

.text
    .global _start

_start:
    nop
    ldli    r3, #34
    ldhi    r4, #0xffff0000
    ldhi    r5, #0x0123ffff
    or      r1, z, #7-2*4  ; -1

    or      r6, z, #loop+7

    bl      #foo

    ldhi    r7, 0x12345678@hi
    or      r7, r7, 0x12345678@lo

    ldwpc   r9, #loop@pc
    ldli    r8, #hello_len
    ldhi    r9, #hello@hi
    add     r9, r9, #hello@lo

    ; A small loop.
loop:
    add     r3, r3, r1
    bz      r3, #loop

    ; Some random vector instructions.
    or      v3, vz, r3
    fmul.h  v3, v3, r1
    add.b/f v3, v3, v1
    ldw     v29, [r9, #4]

    mov     v12, v3
    mov     v13, r1
    mov     v14, #1234

    ; Terminate the program (unconditional jump to PC + 0).
end:
    b       #end

foo:
    seq     r1, r2, #42
    ret


;------------------------------------------------------
; Data section
;------------------------------------------------------

.data

stuff:
    .zero    123

hello:
    .asciz    "Hello, world!\n"
    hello_len = . - hello
