JMP START

int_0_n: db 0
int_0_$T1: db 0
int_0_$T2: db 0
int_0_$T3: db 0
int_0_$T4: db 0
int_0_$T5: db 0
int_0_$T6: db 0
int_0_$T7: db 0
int_0_$T8: db 0
int_2_inp: db 0
int_2_res: db 0
int_2_$T9: db 0

START:
JMP main

fib:

MVI A, 1
STA int_0_$T1

MVI A, 0
MOV B, A
LDA int_0_n
CMP B
JZ L1

MVI A, 0
STA int_0_$T1

L1:

MVI A, 1
STA int_0_$T2

MVI A, 1
MOV B, A
LDA int_0_n
CMP B
JZ L2

MVI A, 0
STA int_0_$T2

L2:

LDA int_0_$T2
MOV B, A
LDA int_0_$T1
ORA B
STA int_0_$T3

MVI A, 0
MOV B, A
LDA int_0_$T3
CMP B
JZ L3

; RET IS NOT IMPLEMENTED

JMP L4

L3:

L4:

MVI A, 1
MOV B, A
LDA int_0_n
SUB B
STA int_0_$T4

; PARAM IS NOT IMPLEMENTED

; CALL IS NOT IMPLEMENTED

MVI A, 2
MOV B, A
LDA int_0_n
SUB B
STA int_0_$T6

; PARAM IS NOT IMPLEMENTED

; CALL IS NOT IMPLEMENTED

LDA int_0_$T7
MOV B, A
LDA int_0_$T5
ADD B
STA int_0_$T8

; RET IS NOT IMPLEMENTED

; RET IS NOT IMPLEMENTED

main:

IN 0
STA int_2_inp

; PARAM IS NOT IMPLEMENTED

; CALL IS NOT IMPLEMENTED

LDA int_2_$T9
STA int_2_res

LDA int_2_res
OUT 1

; RET IS NOT IMPLEMENTED

HLT
