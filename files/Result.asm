JMP START

int_0_x: db 0
int_0_y: db 0
int_0_tmp: db 0
int_0_$T1: db 0
int_0_$T2: db 0
int_0_$T3: db 0
int_2_a: db 0
int_2_b: db 0
int_2_result: db 0
int_2_$T4: db 0

START:
JMP main

func:

MVI A, 0
STA int_0_tmp

LDA int_0_y
MOV B, A
LDA int_0_x
ADD B
STA int_0_$T1

MVI A, 2
MOV C, A
LDA int_0_$T1
MOV B, A
MVI A, 0
CMP C
JZ L5
JMP L4

L4:
ADD B
DCR C
JZ L5
JMP L4

L5:
STA int_0_$T2
LDA int_0_$T2
STA int_0_tmp

MVI A, 1
STA int_0_$T3

LDA int_0_y
MOV B, A
LDA int_0_tmp
CMP B
JM L1

MVI A, 0
STA int_0_$T3

L1:

MVI A, 0
MOV B, A
LDA int_0_$T3
CMP B
JZ L2

LDA int_0_y
STA int_0_tmp

JMP L3

L2:

L3:

LDA int_0_tmp
LXI H, 
DAD SP
MOV M, A

POP B
POP B
POP B
POP B

RET

MVI A, 0
LXI H, 
DAD SP
MOV M, A

POP B
POP B
POP B
POP B

RET

main:

IN 0
STA int_2_a

IN 0
STA int_2_b

LDA int_2_$T4
STA int_2_result

LDA int_2_result
OUT 1

MVI A, 0
LXI H, 
DAD SP
MOV M, A

POP B
POP B
POP B
POP B

RET

MVI A, 0
LXI H, 
DAD SP
MOV M, A

POP B
POP B
POP B
POP B

RET

HLT
