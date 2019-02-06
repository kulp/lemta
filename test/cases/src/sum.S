reset:
    rjmp main
.global main
.align 5
main:
    add r18, r16
    add r18, r17
    ldi r19, 1 ; signify that we are done

    rjmp .-2 ; loop forever
