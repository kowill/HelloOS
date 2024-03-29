[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api007.nas"]

GLOBAL _api_boxfilwin

[SECTION .text]

_api_boxfilwin:     ; void api_boxfilwin(int win, int x0, int y0, int x1, int y1, int col);
    PUSH EDI
    PUSH ESI
    PUSH EBP
    PUSH EBX
    MOV EDX, 7
    MOV EBX, [ESP+20]
    MOV EAX, [ESP+24]
    MOV ECX, [ESP+28]
    MOV ESI, [ESP+32]
    MOV EDI, [ESP+36]
    MOV EBP, [ESP+40]
    INT 0x40
    POP EBX
    POP EBP
    POP ESI
    POP EDI
    RET
