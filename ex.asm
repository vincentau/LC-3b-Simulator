		.ORIG	x3000

;Test for AND and ADD
		AND R0, R0, #0
		AND R1, R1, #0
		AND R2, R2, #0
		AND R3, R3, #0
		ADD R1, R1, #1		;R1 = 0x0001
		ADD R2, R2, #2		;R2 = 0x0002
		ADD R3, R1, R2		;R3 = 0x0003
		ADD R0, R0, #-1		;R0 = 0xFFFF
		AND R3, R3, R0		;R3 should stay the same
		ADD R0, R0, #-8		;R0 = 0xFFF7
		ADD R3, R3, R0		;R3 = 0xFFFA
		AND R3, R3, R2		;R3 = 0x0002
		ADD R0, R0, #9		;R0 = 0x0000
		ADD R3, R3, #1		;R3 = 0x0003

;Run 14 to get to here
;Tests for NOT and XOR
		NOT R0, R0			;R0 = 0xFFFF (-1)
		NOT R1, R1			;R1 = 0xFFFE (-2)
		XOR R1, R0, R1		;R1 = 0x0001
		XOR R0, R0, #-1		;R0 = 0x0000
		XOR R4, R1, #7		;R4 = 0x0006

;Run 19 to get to here
;Tests for Branches
;P = 1 going in
		BRp	P
N		ADD R0, R0, #1
		BRz Z
P		ADD R0, R0, #-1
		BRn N
Z		BRnp P




		.END
