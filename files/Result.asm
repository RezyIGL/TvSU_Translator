JMP START

int_0_a: db 0
int_0_b: db 0
int_0_c: db 0
int_0_k: db 0
int_0_$T1: db 0
int_if_0_0_j: db 6
int_if_0_0_$T2: db 0
int_else_0_0_z: db 3
int_else_0_0_$T3: db 0
int_0_$T4: db 0
int_if_0_1_j: db 6
int_if_0_1_$T5: db 0
int_else_0_1_z: db 3
int_else_0_1_$T6: db 0
int_0_$T7: db 0
int_if_0_2_j: db 6
int_if_0_2_$T8: db 0
int_else_0_2_z: db 3
int_else_0_2_$T9: db 0
int_0_$T10: db 0
int_if_0_3_j: db 6
int_if_0_3_$T11: db 0
int_else_0_3_z: db 3
int_else_0_3_$T12: db 0

START:
JMP main

main:

MVI A, 4
STA int_0_b

MVI A, 1
STA int_0_$T1

MVI A, 5
MOV B, A
LDA int_0_a
CMP B
JZ L1

MVI A, 0
STA int_0_$T1

L1:

MVI A, 0
MOV B, A
LDA int_0_$T1
CMP B
JZ L2

MVI A, 6
STA int_if_0_0_j

LDA int_if_0_0_j
MOV B, A
LDA int_0_b
ADD B
STA int_if_0_0_$T2

LDA int_if_0_0_$T2
STA int_0_b

JMP L3

L2:

MVI A, 3
STA int_else_0_0_z

LDA int_else_0_0_z
MOV B, A
LDA int_0_b
ADD B
STA int_else_0_0_$T3

LDA int_else_0_0_$T3
STA int_0_b

L3:

MVI A, 1
STA int_0_$T4

MVI A, 3
MOV B, A
LDA int_0_c
CMP B
JZ L4

MVI A, 0
STA int_0_$T4

L4:

MVI A, 0
MOV B, A
LDA int_0_$T4
CMP B
JZ L5

MVI A, 6
STA int_if_0_1_j

LDA int_if_0_1_j
MOV B, A
LDA int_0_b
ADD B
STA int_if_0_1_$T5

LDA int_if_0_1_$T5
STA int_0_b

JMP L6

L5:

MVI A, 3
STA int_else_0_1_z

LDA int_else_0_1_z
MOV B, A
LDA int_0_b
ADD B
STA int_else_0_1_$T6

LDA int_else_0_1_$T6
STA int_0_b

L6:

MVI A, 1
STA int_0_$T7

MVI A, 1
MOV B, A
LDA int_0_b
CMP B
JZ L7

MVI A, 0
STA int_0_$T7

L7:

MVI A, 0
MOV B, A
LDA int_0_$T7
CMP B
JZ L8

MVI A, 6
STA int_if_0_2_j

LDA int_if_0_2_j
MOV B, A
LDA int_0_b
ADD B
STA int_if_0_2_$T8

LDA int_if_0_2_$T8
STA int_0_b

JMP L9

L8:

MVI A, 3
STA int_else_0_2_z

LDA int_else_0_2_z
MOV B, A
LDA int_0_b
ADD B
STA int_else_0_2_$T9

LDA int_else_0_2_$T9
STA int_0_b

L9:

MVI A, 1
STA int_0_$T10

MVI A, 4
MOV B, A
LDA int_0_k
CMP B
JZ L10

MVI A, 0
STA int_0_$T10

L10:

MVI A, 0
MOV B, A
LDA int_0_$T10
CMP B
JZ L11

MVI A, 6
STA int_if_0_3_j

LDA int_if_0_3_j
MOV B, A
LDA int_0_b
ADD B
STA int_if_0_3_$T11

LDA int_if_0_3_$T11
STA int_0_b

JMP L12

L11:

MVI A, 3
STA int_else_0_3_z

LDA int_else_0_3_z
MOV B, A
LDA int_0_b
ADD B
STA int_else_0_3_$T12

LDA int_else_0_3_$T12
STA int_0_b

L12:

; RET IS NOT IMPLEMENTED

HLT
