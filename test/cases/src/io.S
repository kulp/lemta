reset:
    rjmp main
.global main
.align 5
main:
    in r16, 12
    mov r17, r16
    sec
    adc r17, r17
    out 14, r17

    rjmp .-2 ; loop forever
