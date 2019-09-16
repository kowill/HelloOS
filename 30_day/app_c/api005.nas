[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[BITS 32]
[FILE "api005.nas"]

GLOBAL _api_openwin

[SECTION .text]

_api_openwin:   ; int api_openwin(char *buf, int xsiz, int ysiz, int col_inv, char *title);
    PUSH EDI
    PUSH ESI
    PUSH EBX
    MOV EDX, 5
    MOV EBX, [ESP+16]
    MOV ESI, [ESP+20]
    MOV EDI, [ESP+24]
    MOV EAX, [ESP+28]
    MOV ECX, [ESP+32]
    INT 0x40
    POP EBX
    POP ESI
    POP EDI
    RET
