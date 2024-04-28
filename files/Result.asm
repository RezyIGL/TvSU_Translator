JMP START

int_0_n: db 11
int_0_s: db 0
int_0_T1: db 12
int_for_0_0_i: db 0
int_for_0_0_$T1: db 0
int_for_0_0_$T2: db 0
int_for_0_0_$T3: db 0
int_0_i: db 12
int_for_0_1_j: db 0
int_for_0_1_$T4: db 0

START:
JMP main

main:

MVI A, 11
STA int_0_n

MVI A, 0
STA int_0_s

MVI A, 12
STA int_0_T1

MVI A, 0
STA int_for_0_0_i

L1:

MVI A, 1
STA int_for_0_0_$T1

LDA int_0_n
MOV B, A
LDA int_for_0_0_i
CMP B
JM L5
JZ L5

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

LDA int_for_0_0_i
MOV B, A
LDA int_0_s
ADD B
STA int_for_0_0_$T3

LDA int_for_0_0_$T3
STA int_0_s

LDA int_for_0_0_i
STA int_0_T1

JMP L2

L4:

MVI A, 12
STA int_0_i

MVI A, 0
STA int_for_0_1_j

L6:

MVI A, 1
STA int_for_0_1_$T4

MVI A, 12
MOV B, A
LDA int_0_i
CMP B
JM L10
JZ L10

MVI A, 0
STA int_for_0_1_$T4

L10:

MVI A, 0
MOV B, A
LDA int_for_0_1_$T4
CMP B
JZ L9

JMP L8

L7:

MVI A, 1
MOV B, A
LDA int_0_i
ADD B
STA int_0_i

JMP L6

L8:

JMP L7

L9:

LDA int_0_s
; RET IS NOT IMPLEMENTED

HLT
