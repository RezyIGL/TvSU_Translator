ORG 8000H

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

MVI A, 1
LXI H, 6
DAD SP
MOV M, A

MVI A, 1
MOV B, A
LXI H, 8
DAD SP
MOV A, M
CMP B
JM L1
JZ L1

MVI A, 0
LXI H, 6
DAD SP
MOV M, A

L1:

MVI A, 0
MOV B, A
LXI H, 6
DAD SP
MOV A, M
CMP B
JZ L2

LXI H, 12
DAD SP
MVI M, 1

POP B
POP B
POP B
POP B

RET

JMP L3

L2:

L3:

MVI A, 1
MOV B, A
LXI H, 8
DAD SP
MOV A, M
SUB B
LXI H, 4
DAD SP
MOV M, A

PUSH B
PUSH B

LXI H, 10
DAD SP
MOV A, M
LXI H, 0
DAD SP
MOV M, A

CALL func

POP B
POP B

MOV A, C

LXI H, 2
DAD SP
MOV M, A

LXI H, 2
DAD SP
MOV A, M
MOV C, A
LXI H, 8
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
LXI H, 0
DAD SP
MOV M, A

LXI H, 0
DAD SP
MOV A, M
LXI H, 12
DAD SP
MOV M, A

POP B
POP B
POP B
POP B

RET

LXI H, 12
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

PUSH B
PUSH B

MVI A, 7
LXI H, 0
DAD SP
MOV M, A

CALL func

POP B
POP B

MOV A, C

LXI H, 0
DAD SP
MOV M, A

LXI H, 0
DAD SP
MOV A, M
LXI H, 4
DAD SP
MOV M, A

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

