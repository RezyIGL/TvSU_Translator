JMP START

int_0_i: db 0
int_0_n: db 11
int_0_s: db 0
int_0_$T1: db 0
int_0_t: db 0
int_0_$T2: db 0
int_0_$T3: db 0

START:
JMP main

main:

MVI A, 11
STA int_0_n

MVI A, 0
STA int_0_s

MVI A, 1
STA int_0_i

L1:

MVI A, 1
STA int_0_$T1

LDA int_0_n
MOV B, A
LDA int_0_i
CMP B
JM L5
JZ L5

MVI A, 0
STA int_0_$T1

L5:

MVI A, 0
MOV B, A
LDA int_0_$T1
CMP B
JZ L4

JMP L3

L2:

MVI A, 1
MOV B, A
LDA int_0_i
ADD B
STA int_0_i

JMP L1

L3:

MVI A, 0
STA int_0_t

LDA int_0_i
MOV B, A
LDA int_0_t
ADD B
STA int_0_$T2

LDA int_0_$T2
STA int_0_t

LDA int_0_s
MOV B, A
LDA int_0_t
ADD B
STA int_0_$T3

LDA int_0_$T3
STA int_0_t

LDA int_0_t
STA int_0_s

JMP L2

L4:

; RET IS NOT IMPLEMENTED

HLT
