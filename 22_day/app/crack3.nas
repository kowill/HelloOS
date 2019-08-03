[INSTRSET "i486p"]
[BITS 32]
    MOV AL, 0x34
    OUT 0x43, AL
    MOV AL, 0xff
    OUT 0x40, AL
    MOV AL, 0xff
    OUT 0x40, AL
    MOV EDX, 4
    INT 0x40
    