; haribote-os

    ORG 0xc200
    MOV AL, 0x13    ; VGA 320*200*8bit color
    MOV AH, 0x00
    INT 0x10

fin:
    HLT
    JMP fin