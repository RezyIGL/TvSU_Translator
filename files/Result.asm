ORG 8000H
int_dev_a: db 0

ORG 0

LXI H, 0
SPHL
PUSH B
CALL main
END

@ DIV ...
@ PRINT ...

func:

PUSH B
PUSH B
PUSH B
PUSH B

LXI H, 10
DAD SP
MOV A, M
MOV B, A
LXI H, 12
DAD SP
MOV A, M
ADD B
LXI H, 4
DAD SP
MOV M, A

MVI A, 2
MOV C, A
LXI H, 4
DAD SP
MOV A, M
MOV B, A
LDA int_dev_func
CMP C
JZ L5
JMP L4

L4:
ADD B
DCR C
JZ L5
JMP L4

L5:
LXI H, 2
DAD SP
MOV M, A

LXI H, 2
DAD SP
MOV A, M
LXI H, 6
DAD SP
MOV M, A

MVI A, 1
LXI H, 0
DAD SP
MOV M, A

LXI H, 10
DAD SP
MOV A, M
MOV B, A
LXI H, 6
DAD SP
MOV A, M
CMP B
JM L1

MVI A, 0
LXI H, 0
DAD SP
MOV M, A

L1:

MVI A, 0
MOV B, A
LXI H, 0
DAD SP
MOV A, M
CMP B
JZ L2

LXI H, 10
DAD SP
MOV A, M
LXI H, 6
DAD SP
MOV M, A

JMP L3

L2:

L3:

LXI H, 6
DAD SP
MOV A, M
LXI H, 14
DAD SP
MOV M, A

POP B
POP B
POP B
POP B

RET

LXI H, 14
DAD SP
MVI M, 0

POP B
POP B
POP B
POP B

RET

main:

PUSH B
PUSH B
PUSH B

IN 0
STA int_dev_a

IN 0
LXI H, 4
DAD SP
MOV M, A

PUSH B
PUSH B
PUSH B

LXI H, 10
DAD SP
MOV A, M
LXI H, 0
DAD SP
MOV M, A

LDA int_dev_a
LXI H, 2
DAD SP
MOV M, A

CALL func

POP B
POP B
POP B

MOV A, C

LXI H, 0
DAD SP
MOV M, A

LXI H, 0
DAD SP
MOV A, M
LXI H, 2
DAD SP
MOV M, A

LXI H, 2
DAD SP
MOV A, M
OUT 1

LXI H, 8
DAD SP
MVI M, 0

POP B
POP B
POP B

RET

LXI H, 8
DAD SP
MVI M, 0

POP B
POP B
POP B

RET

