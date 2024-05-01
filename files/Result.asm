JMP START

int_1_i: db 0
int_1_$T1: db 0
int_1_$T2: db 0
int_2_j: db 0
int_2_$T3: db 0
int_2_$T4: db 0
int_2_a: db 0
int_2_$T5: db 0

START:
JMP main

main:

MVI A, 0
STA int_1_i

L1:

MVI A, 1
STA int_1_$T1

MVI A, 10
MOV B, A
LDA int_1_i
CMP B
JM L5

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

MVI A, 1
MOV B, A
LDA int_1_i
ADD B
STA int_1_$T2

LDA int_1_$T2
STA int_1_i

JMP L1

L3:

MVI A, 1
STA int_2_j

L6:

MVI A, 1
STA int_2_$T3

MVI A, 10
MOV B, A
LDA int_2_j
CMP B
JM L10

MVI A, 0
STA int_2_$T3

L10:

MVI A, 0
MOV B, A
LDA int_2_$T3
CMP B
JZ L9

JMP L8

L7:

MVI A, 1
MOV B, A
LDA int_2_j
ADD B
STA int_2_$T4

LDA int_2_$T4
STA int_2_j

JMP L6

L8:

LDA int_2_j
MOV C, A
LDA int_1_i
MOV B, A
MVI A, 0
CMP C
JZ L12
JMP L11

L11:
ADD B
DCR C
JZ L12
JMP L11

L12:
STA int_2_$T5
LDA int_2_$T5
STA int_2_a

JMP L7

L9:

JMP L2

L4:

; RET IS NOT IMPLEMENTED

HLT
