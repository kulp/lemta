reset:
    rjmp main
.global main
.align 5
main:
    break
    ldi r19, 1 ; signify that we are done

    rjmp .-2 ; loop forever
