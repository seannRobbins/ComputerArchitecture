	;Seann Robbins: snr2367
	;Aniketh Deverasetty: ad48322
	
	.ORIG x3000
	AND R3, R3, #0 ; overflow storage value
	LEA R0, first
	LDW R0, R0, #0
	LDB R1, R0, #0
	BRp pos ; first byte is pos
	BRn neg ; first byte is neg
	BRz zero ; no overflow can occur
pos LEA R0, second
	LDW R0, R0, #0
	LDB R2, R0, #0
	BRp twoPos
	ADD R0, R1, R2
	BR end
neg LEA R0, second
	LDW R0, R0, #0
	LDB R2, R0, #0
	BRn twoNegs
	ADD R0, R1, R2
	BR end
zero LEA R0, second
	LDW R0, R0, #0
	LDB R2, R0, #0
	ADD R0, R1, R2
	BR end
twoPos ADD R0, R1, R2
	BRnz over
	BRp end
twoNegs ADD R0, R1, R2
	BRzp over
	BRn end
over ADD R3, R3, #1 ; change the overflow byte from 0 to 1
end LEA R4, store
	LDW R4, R4, #0
	STB R0, R4, #0 ; Store the sum of the two bytes
	LEA R4, overflow
	LDW R4, R4, #0
	STB R3, R4, #0 ; store the value of the overflow byte
first .FILL x3100
second .FILL x3101
store .FILL x3102
overflow .FILL x3103
	.END