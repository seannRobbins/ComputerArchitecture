.ORIG x3000
D BRn D
C LEA R7, C
.FILL #-1
ADD R3, R1, R4
ADD R3, R1, #5
ADD R3, R1, x-5
;first set of branches

BR B
BRn B
BRnz B
BRnzp B
BRz B
BRzp B
BRp B
BRnp B
AND R3, R4, R3
AND R1, R3, x8
B AND R1, R3, #-4 ;first label b
JMP R5
JSR x10
JSR x-15
JSR A
JSRR R3
; LDB R3, R3, x5
; LDB R4, R4, x-7
; LDW R3, R3, x5
; LDW R4, R4, x-7
;second set of branches

BR A
BRn A
BRnz A
BRnzp A
BRz A
BRzp A
BRp A
BRnp A
C LEA R1, C
LEA R1, B
LEA R1, x10
LEA R1, x-15
NOT R4, R1
RET
RTI
A 
; LSHF R3, R1, x5 ; second label a
; RSHFL R3, R1, x5
; RSHFA R3, R1, x5
; STB R3, R3, x5
; STB R4, R4, x-7
; STW R3, R3, x5
; STW R4, R4, x-7
TRAP x26
; XOR R3, R1, R4
; XOR R3, R1, #5
; XOR R3, R1, x-5
.FILL x3000
.END