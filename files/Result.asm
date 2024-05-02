JMP START

int_0_n: db 10
int_0_s: db 0
int_1_i: db 0
int_1_$T1: db 0
int_1_$T2: db 0
int_1_$T3: db 0

START:
JMP main

main:

MVI A, 10
STA int_0_n

MVI A, 0
STA int_0_s

MVI A, 0
STA int_1_i

L1:

MVI A, 1
STA int_1_$T1

LDA int_0_n
MOV B, A
LDA int_1_i
CMP B
JM L5
JZ L5

MVI A, 0
STA int_1_$T1

L5:

MVI A, 0
MOV B, A
LDA int_1_$T1
CMP B
JZ L4

JMP L3

L2:

LDA int_1_i
STA int_1_$T2

MVI A, 1
MOV B, A
LDA int_1_i
ADD B
STA int_1_i

JMP L1

L3:

LDA int_1_i
MOV B, A
LDA int_0_s
ADD B
STA int_1_$T3

LDA int_1_$T3
STA int_0_s

JMP L2

L4:

LDA int_0_s
OUT 1

; RET IS NOT IMPLEMENTED

HLT
