JMP START

int_0_n: db 11
int_0_s1: db 0
int_0_s2: db 0
int_1_i: db 0
int_1_$T1: db 0
int_1_$T2: db 0
int_2_i: db 0
int_2_$T3: db 0
int_2_$T4: db 0

START:
JMP main

main:

MVI A, 11
STA int_0_n

MVI A, 0
STA int_0_s1

MVI A, 0
STA int_0_s2

MVI A, 1
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

MVI A, 1
MOV B, A
LDA int_1_i
ADD B
STA int_1_i

JMP L1

L3:

LDA int_1_i
MOV B, A
LDA int_0_s1
ADD B
STA int_1_$T2

LDA int_1_$T2
STA int_0_s1

JMP L2

L4:

LDA int_0_n
STA int_2_i

L6:

MVI A, 1
STA int_2_$T3

MVI A, 0
MOV B, A
LDA int_2_i
CMP B
JP L10

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
LDA int_2_i
SUB B
STA int_2_i

JMP L6

L8:

LDA int_2_i
MOV B, A
LDA int_0_s2
ADD B
STA int_2_$T4

LDA int_2_$T4
STA int_0_s2

JMP L7

L9:

LDA int_0_s1
OUT 1

LDA int_0_s2
OUT 1

; RET IS NOT IMPLEMENTED

HLT
