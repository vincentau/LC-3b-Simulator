	.ORIG x4000
    
    AND R0, R0, #0
    AND R3, R3, #0
    AND R4, R4, #0
    AND R5, R5, #0
    ADD R3, R3, #3
    ADD R4, R4, #-8
    LEA R0, M
    JSRR R0
    
    HALT

M	RSHFA R4, R4, #1
	ADD R3, R3, #-1
	BRnp M
	RET


A   .FILL x4000
B 	.FILL #-4	
    
    .END