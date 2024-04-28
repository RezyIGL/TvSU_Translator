JMP START

int_for_0_0_i: db 0
int_for_0_0_$T1: db 0
int_for_0_0_$T2: db 0

START:
JMP main

main:

MVI A, 0
STA int_for_0_0_i

L1:

MVI A, 1
STA int_for_0_0_$T1

MVI A, 5
MOV B, A
LDA int_for_0_0_i
CMP B
JM L5

MVI A, 0
STA int_for_0_0_$T1

L5:

MVI A, 0
MOV B, A
LDA int_for_0_0_$T1
CMP B
JZ L4

JMP L3

L2:

MVI A, 1
MOV B, A
LDA int_for_0_0_i
ADD B
STA int_for_0_0_$T2

LDA int_for_0_0_$T2
STA int_for_0_0_i

JMP L1

L3:

JMP L2

L4:

; RET IS NOT IMPLEMENTED

HLT
