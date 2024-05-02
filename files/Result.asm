JMP START

int_0_a: db 0
int_0_$T1: db 0
int_0_$T2: db 0

START:
JMP main

main:

MVI A, 0
STA int_0_a

LDA int_0_a
STA int_0_$T1

MVI A, 1
MOV B, A
LDA int_0_a
ADD B
STA int_0_a

LDA int_0_a
STA int_0_$T2

MVI A, 1
MOV B, A
LDA int_0_a
SUB B
STA int_0_a

L1:

MVI A, 0
MOV B, A
MVI A, 
CMP B
JZ L4

JMP L3

L2:

JMP L1

L3:

JMP L2

L4:

; RET IS NOT IMPLEMENTED

HLT
