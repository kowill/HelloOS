[BITS 32]
    MOV AL, 'A'
    CALL 0x2447
fin:
    HLT
    JMP fin
