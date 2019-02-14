reset:
    rjmp main
.global main
.align 5
main:
    in r16, 12

    rjmp .-2 ; loop forever
