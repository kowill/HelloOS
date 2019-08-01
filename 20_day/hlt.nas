[BITS 32]
    MOV AL, 'A'
    CALL 2*8:0x2447
fin:
    HLT
    JMP fin
