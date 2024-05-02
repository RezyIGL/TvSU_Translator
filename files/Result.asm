JMP START

int_0_a: db 1

START:
JMP main

main:

MVI A, 1
STA int_0_a

MVI A, 1
MOV B, A
LDA int_0_a
SUB B
STA int_0_a

MVI A, 1
MOV B, A
LDA int_0_a
ADD B
STA int_0_a

MVI A, 1
MOV B, A
LDA int_0_a
SUB B
STA int_0_a

MVI A, 1
MOV B, A
LDA int_0_a
SUB B
STA int_0_a

; RET IS NOT IMPLEMENTED

HLT
