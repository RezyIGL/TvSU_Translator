JMP START

int_0_n: db 11
int_0_s1: db 0
int_0_s2: db 0
int_0_s3: db 0
int_0_i: db 0
int_for_0_0_$T1: db 0
int_for_0_0_$T2: db 0
int_for_0_0_$T3: db 0
int_for_0_0_$T4: db 0
int_for_0_1_i: db 0
int_for_0_1_$T5: db 0
int_for_0_1_$T6: db 0
int_for_0_1_$T7: db 0
int_for_0_1_$T8: db 0
int_for_0_2_i: db 0
int_for_0_2_$T9: db 0
int_for_0_2_$T10: db 0

START:
JMP main

main:

MVI A, 11
STA int_0_n

MVI A, 0
STA int_0_s1

MVI A, 0
STA int_0_s2

MVI A, 0
STA int_0_s3

MVI A, 0
STA int_0_i

L1:

MVI A, 1
MOV B, A
LDA int_0_n
ADD B
STA int_for_0_0_$T1

MVI A, 1
STA int_for_0_0_$T2

LDA int_for_0_0_$T1
MOV B, A
LDA int_0_i
CMP B
JM L5

MVI A, 0
STA int_for_0_0_$T2

L5:

MVI A, 0
MOV B, A
LDA int_for_0_0_$T2
CMP B
JZ L4

JMP L3

L2:

MVI A, 1
MOV B, A
LDA int_0_i
ADD B
STA int_for_0_0_$T3

LDA int_for_0_0_$T3
STA int_0_i

JMP L1

L3:

LDA int_0_i
MOV B, A
LDA int_0_s1
ADD B
STA int_for_0_0_$T4

LDA int_for_0_0_$T4
STA int_0_s1

JMP L2

L4:

MVI A, 0
STA int_for_0_1_i

L6:

MVI A, 1
MOV B, A
LDA int_0_n
ADD B
STA int_for_0_1_$T5

MVI A, 1
STA int_for_0_1_$T6

LDA int_for_0_1_$T5
MOV B, A
LDA int_for_0_1_i
CMP B
JM L10
JZ L10

MVI A, 0
STA int_for_0_1_$T6

L10:

MVI A, 0
MOV B, A
LDA int_for_0_1_$T6
CMP B
JZ L9

JMP L8

L7:

MVI A, 1
MOV B, A
LDA int_for_0_1_i
ADD B
STA int_for_0_1_$T7

LDA int_for_0_1_$T7
STA int_for_0_1_i

JMP L6

L8:

LDA int_for_0_1_i
MOV B, A
LDA int_0_s2
ADD B
STA int_for_0_1_$T8

LDA int_for_0_1_$T8
STA int_0_s2

JMP L7

L9:

MVI A, 0
STA int_for_0_2_i

L11:

MVI A, 1
STA int_for_0_2_$T9

LDA int_0_n
MOV B, A
LDA int_for_0_2_i
CMP B
JM L15
JZ L15

MVI A, 0
STA int_for_0_2_$T9

L15:

MVI A, 0
MOV B, A
LDA int_for_0_2_$T9
CMP B
JZ L14

JMP L13

L12:

MVI A, 1
MOV B, A
LDA int_for_0_2_i
ADD B
STA int_for_0_2_i

JMP L11

L13:

LDA int_for_0_2_i
MOV B, A
LDA int_0_s3
ADD B
STA int_for_0_2_$T10

LDA int_for_0_2_$T10
STA int_0_s3

JMP L12

L14:

LDA int_0_s1
; RET IS NOT IMPLEMENTED

HLT
