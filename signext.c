#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Low16bits(x) ((x) & 0xFFFF)
#define LC_3b_REGS 8

typedef struct System_Latches_Struct {
	int PC,   /* program counter */
	    N,    /* n condition bit */
	    Z,    /* z condition bit */
	    P;    /* p condition bit */
	int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

int sext(int num, int nbits);
void clearNZP(void);
void setNZP(int num);
int processVal(int val, int nbits);
void add(int instr);


/* Start main function */
int main(int argc, char *argv[]) {
	int instr = 0x1B3E;
	add(instr);
}


/* sign extend an integer to 16 bits --NOT SURE IF NECESSARY */
int sext(int num, int nbits) {
	switch (nbits) {
	case 4:
		num &= 0x000F;
		if (0x0008 & num)
			num = Low16bits(num |= 0xFFF0);
		break;

	case 5:
		num &= 0x001F;
		if (0x0010 & num)
			num = Low16bits(num |= 0xFFE0);
		break;

	}
}

/* Sets NZP bits to 0 */
void clearNZP() {
	NEXT_LATCHES.N = 0;
	NEXT_LATCHES.Z = 0;
	NEXT_LATCHES.P = 0;
}

/* Sets the condition code given value num */
void setNZP(int num) {
	if (num < 0) {
		NEXT_LATCHES.N = 1;
		return;
	}

	if (num == 0) {
		NEXT_LATCHES.Z = 1;
		return;
	}

	if (num > 0) {
		NEXT_LATCHES.P = 1;
		return;
	}
}

/* takes 2's complement of constant value if necessary */
int procTwosComp(int val, int nbits) {
	switch (nbits) {
	case 5:		/* imm5 */
		if (val > 15) {
			val = 16 - (val & 0xF);
			val = -val;
		}
		break;

	case 6:		/* boffset6 and offset6 */
		if (val > 31) {
			val = 32 - (val & 0x1F);
			val = -val;
		}
		break;

	case 9:		/* PCoffset9 */
		if (val > 255) {
			val = 256 - (val & 0xFF);
			val = -val;
		}
		break;

	case 11:	/* PCoffset11 */
		if (val > 1023) {
			val = 1024 - (val & 0x3FF);
			val = -val;
		}
		break;

	default:
		break;
	}
	return val;
}

/* ADD instruction simulation */
void add(int instr) {
	clearNZP();
	int dr, sr1, sr2, imm, sum;

	dr = (instr & 0x0E00) >> 9;
	sr1 = (instr & 0x01C0) >> 6;
	printf("DR: R%d\n", dr);
	printf("SR1: R%d\n", sr1);

	if (!(0x0020 & instr)) {    /* if steering bit is 0 */
		sr2 = (instr & 0x0007);
		printf("SR2: R%d\n", sr2);
		sum = CURRENT_LATCHES.REGS[sr1] + CURRENT_LATCHES.REGS[sr2];
		printf("Sum = %d\n", sum);
	} else {
		imm = processImmVal(instr & 0x001F);
		printf("Imm: #%d, 0x%0.4X\n", imm, imm);
		sum = CURRENT_LATCHES.REGS[sr1] + imm;
	}
	printf("Sum = %d\n", sum);

	/* Set condition codes here */
	NEXT_LATCHES.REGS[dr] = Low16bits(sum);
	printf("%d\n", NEXT_LATCHES.REGS[dr]);
}

/* AND intruction simulation */
void and(int instr) {
	
}
