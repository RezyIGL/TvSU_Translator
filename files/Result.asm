JMP START

int_0_i: db 0
int_0_$T1: db 0
int_0_a: db 3

START:
JMP main

main:

MVI A, 1
STA main_int_$T1

MVI A, 2
MOV B, A
MVI A, 1
CMP B
JM L1

MVI A, 0
STA main_int_$T1

L1:

MVI A, 0
MOV B, A
LDA main_int_$T1
CMP B
JZ L2

JMP L3

L2:

L3:

; RET IS NOT IMPLEMENTED

main:
HLT
