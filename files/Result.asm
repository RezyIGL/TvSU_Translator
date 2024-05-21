ORG 8000H

ORG 0

LXI H, 0
SPHL
PUSH B
CALL main
END

@ DIV ...
@ PRINT ...

main:


; Strings are not implemented here
LXI H, 2
DAD SP
MVI M, 0


RET

