; naskfunc

[FORMAT "WCOFF"]
[INSTRSET "i486p"]	
[BITS 32]

[FILE "naskfunc.nas"]
    GLOBAL  _io_hlt, _write_mem8

[SECTION .text]
_io_hlt:        ; void io_hlt(void);
    HLT
    RET

_write_mem8:    ; void write_mem8(int addr, int data);
    MOV ECX, [ESP+4]
    MOV AL, [ESP+8]
    MOV [ECX], AL
    RET 