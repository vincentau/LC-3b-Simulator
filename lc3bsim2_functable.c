/*
    Name 1: Joshua Smith
    UTEID 1: jds5228
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
*/

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;  /* run bit */


typedef struct System_Latches_Struct {

  int PC,   /* program counter */
      N,    /* n condition bit */
      Z,    /* z condition bit */
      P;    /* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
      RUN_BIT = FALSE;
      printf("Simulator halted\n\n");
      break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
  int k;

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch (buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
      rdump(dumpsim_file);
    else {
      scanf("%d", &cycles);
      run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
  int i;

  for (i = 0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
      printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
      exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while (*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;
  NEXT_LATCHES = CURRENT_LATCHES;

  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here                  */

/***************************************************************/

/* All code below is original and was written by Joshua Smith */
#define Low8Bits(x)   (x & 0x00FF)

#define BR        0
#define ADD       1
#define LDB       2
#define STB       3
#define JSR       4
#define AND       5
#define LDW       6
#define STW       7
#define RTI       8
#define NOT_XOR   9
#define JMP_RET   12
#define SHF       13
#define LEA       14
#define TRAP      15

void add(int instr);
void and(int instr);
void branch(int instr);
void jmp_ret(int instr);
void jsr(int instr);
void ldb(int instr);
void ldw(int instr);
void lea(int instr);
void not_xor(int instr);
void shf(int instr);
void stb(int instr);
void stw(int instr);
void trap(int instr);
void rti(int instr);
void nop(int instr); /* No op func for space holding */
void clearNZP(void);
void setNZP(int num);
int procTwosComp(int val, int nbits);

typedef void (*func)(int);
func instrtable[] = {branch, add, ldb, stb, jsr, and, ldw, 
  stw, rti, not_xor, nop, nop, jmp_ret, shf, lea, trap};

void process_instruction() {
  /*  function: process_instruction
   *
   *    Process one instruction at a time
   *       -Fetch one instruction
   *       -Decode
   *       -Execute
   *       -Update NEXT_LATCHES
   */

  int iLowB = Low8Bits(MEMORY[CURRENT_LATCHES.PC / 2][0]);
  int iHighB = Low8Bits(MEMORY[CURRENT_LATCHES.PC / 2][1]);
  int instr = Low16bits((iHighB << 8) | iLowB);

  int opcode = ((instr & 0xF000) >> 12);
  instrtable[opcode](instr);
}

/* Sets NZP bits to 0 */
void clearNZP() {
  NEXT_LATCHES.N = 0;
  NEXT_LATCHES.Z = 0;
  NEXT_LATCHES.P = 0;
}

/* Sets the proper NZP bit given the value num */
void setNZP(int num) {
  NEXT_LATCHES.N = 0;
  NEXT_LATCHES.Z = 0;
  NEXT_LATCHES.P = 0;

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

/* sign extends a 16 bit value to 32 bits */
int sext32(int val, int nbits) {
  if (nbits == 16) {
    if (val & 0x8000) {
      return (val | 0xFFFF0000);
    } 
  }
  return val;
}

/* takes 2's complement of constant value if necessary */
int procTwosComp(int val, int nbits) {
  switch (nbits) {
  case 5:   /* imm5 */
    if (val > 15) {
      val = 16 - (val & 0xF);
      val = -val;
    }
    break;

  case 6:   /* boffset6 and offset6 */
    if (val > 31) {
      val = 32 - (val & 0x1F);
      val = -val;
    }
    break;

  case 9:   /* PCoffset9 */
    if (val > 255) {
      val = 256 - (val & 0xFF);
      val = -val;
    }
    break;

  case 11:  /* PCoffset11 */
    if (val > 1023) {
      val = 1024 - (val & 0x3FF);
      val = -val;
    }
    break;

  default:
  printf("Invalid number of bits\n");
    break;
  }
  return val;
}

/* ADD instruction processing */
void add(int instr) {
  int dr, sr1, sr2, imm, sum;

  dr = (instr & 0x0E00) >> 9;
  sr1 = (instr & 0x01C0) >> 6;

  if (!(0x0020 & instr)) {    /* if steering bit is 0 */
    sr2 = (instr & 0x0007);
    sum = sext32(CURRENT_LATCHES.REGS[sr1], 16) + sext32(CURRENT_LATCHES.REGS[sr2], 16);
  } else {
    imm = procTwosComp((instr & 0x001F), 5);
    sum = sext32(CURRENT_LATCHES.REGS[sr1], 16) + imm;
  }
  
  NEXT_LATCHES.REGS[dr] = Low16bits(sum);
  setNZP(sum);
  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
}

/* AND instruction simulation */
void and(int instr) {
  int dr, sr1, sr2, imm, res;

  dr = (instr & 0x0E00) >> 9;
  sr1 = (instr & 0x01C0) >> 6;

  if (!(0x0020 & instr)) {    /* if steering bit is 0 */
    sr2 = (instr & 0x0007);
    res = sext32(CURRENT_LATCHES.REGS[sr1], 16) & sext32(CURRENT_LATCHES.REGS[sr2], 16);
  } else {
    imm = procTwosComp((instr & 0x001F), 5);
    res = sext32(CURRENT_LATCHES.REGS[sr1], 16) & imm;
  }

  NEXT_LATCHES.REGS[dr] = Low16bits(res);
  setNZP(res);
  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
}

/* BR instruction simulation */
void branch(int instr) {
  int offset = procTwosComp((instr & 0x01FF), 9);

  /* branch if any CC's are met */
  if (((instr & 0x0800) && CURRENT_LATCHES.N) || ((instr & 0x0400) && CURRENT_LATCHES.Z)
    || ((instr & 0x0200) && CURRENT_LATCHES.P)) {
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2 + (offset << 1);
  }

  /* unconditional branch (BR or BRnzp) */
  else if ((!(instr & 0x0800) && !(instr & 0x0400) && !(instr & 0x0200))
    || ((instr & 0x0800) && (instr & 0x0400) && (instr & 0x0200))) {
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2 + (offset << 1);
  }

  else {
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
  }
}

/* JMP and RET instruction simulation */
void jmp_ret(int instr) {
  int baseR = (instr & 0x01C0) >> 6;
  NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[baseR];
}

/* JSR and JSRR instruction simulation */
void jsr(int instr) {
  int temp, baseR, offset;

  /* store incremented PC in R7 */
  temp = CURRENT_LATCHES.PC + 2;

  if (!(instr & 0x0800)) { /* JSRR */
    baseR = (instr & 0x01C0) >> 6;
    NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[baseR];
  } else { /* JSR */
    offset = procTwosComp((instr & 0x07FF), 11);
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2 + (offset << 1);
  }

  NEXT_LATCHES.REGS[7] = temp;
}

/* LDB instruction */
void ldb(int instr) {
  int dr, baseR, offset, addr, temp;

  dr = (instr & 0x0E00) >> 9;
  baseR = (instr & 0x01C0) >> 6;
  offset = procTwosComp((instr & 0x003F), 6);

  addr = (CURRENT_LATCHES.REGS[baseR] + offset);
  temp = (addr % 2 == 0) ? MEMORY[addr >> 1][0] : MEMORY[addr >> 1][1];

  NEXT_LATCHES.REGS[dr] = Low16bits(temp);
  setNZP(temp);
  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
}

/* LDW instruction */
void ldw(int instr) {
  int dr, baseR, offset, addr, temp;

  dr = (instr & 0x0E00) >> 9;
  baseR = (instr & 0x01C0) >> 6;
  offset = procTwosComp((instr & 0x003F), 6);

  addr = Low16bits((CURRENT_LATCHES.REGS[baseR] + (offset << 1)));
  temp = MEMORY[addr >> 1][0] + (MEMORY[addr >> 1][1] << 8);

  NEXT_LATCHES.REGS[dr] = Low16bits(temp);
  setNZP(temp);
  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
}

/* LEA instruction */
void lea(int instr) {
  int dr, offset, temp;
  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;

  dr = (instr & 0x0E00) >> 9;
  offset = procTwosComp((instr & 0x01FF), 9);
  temp = NEXT_LATCHES.PC + (offset << 1);
  NEXT_LATCHES.REGS[dr] = Low16bits(temp);
}

/* NOT and XOR instructions */
void not_xor(int instr) {
  int dr, sr1, sr2, imm, temp;

  dr = (instr & 0x0E00) >> 9;
  sr1 = (instr & 0x01C0) >> 6;

  if (!(instr & 0x0020)) {
    sr2 = instr & 0x0007;
    temp = sext32(CURRENT_LATCHES.REGS[sr1], 16) ^ sext32(CURRENT_LATCHES.REGS[sr2], 16);
  } else {
    imm = procTwosComp((instr & 0x001F), 5);
    temp = sext32(CURRENT_LATCHES.REGS[sr1], 16) ^ imm;
  }

  NEXT_LATCHES.REGS[dr] = Low16bits(temp);
  setNZP(temp);
  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
}

/* SHF instructions */
void shf(int instr) {
  int dr, sr, amount, temp;

  dr = (instr & 0x0E00) >> 9;
  sr = (instr & 0x01C0) >> 6;
  amount = instr & 0x000F;

  if (!(instr & 0x0010)) {
    temp = sext32(CURRENT_LATCHES.REGS[sr], 16) << amount;
  }

  else {
    if (!(instr & 0x0020)) {
      temp = sext32(CURRENT_LATCHES.REGS[sr], 16) >> amount;
    } else {
      int msb = CURRENT_LATCHES.REGS[sr] & 0x8000;
      temp = sext32(CURRENT_LATCHES.REGS[sr], 16) >> amount;
      temp |= msb;
    }
  }

  setNZP(temp);
  NEXT_LATCHES.REGS[dr] = Low16bits(temp);
  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
}

/* STB instruction */
void stb(int instr) {
  int sr, baseR, offset, addr, temp;

  sr = (instr & 0x0E00) >> 9;
  baseR = (instr & 0x01C0) >> 6;
  offset = procTwosComp((instr & 0x003F), 6);

  addr = CURRENT_LATCHES.REGS[baseR] + offset;
  temp = CURRENT_LATCHES.REGS[sr] & 0x00FF;
  MEMORY[addr >> 1][addr % 2] = temp;

  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
}

/* STW instruction */
void stw(int instr) {
  int sr, baseR, offset, addr, temp;

  sr = (instr & 0x0E00) >> 9;
  baseR = (instr & 0x01C0) >> 6;
  offset = procTwosComp((instr & 0x003F), 6);
  printf("\n");
  printf("SR: %d, baseR: %d, Offset: %d\n", sr, baseR, offset);

  addr = CURRENT_LATCHES.REGS[baseR] + (offset << 1);
  printf("Address: 0x%0.4X\n", addr);
  temp = CURRENT_LATCHES.REGS[sr];
  printf("Value to be stored: %d\n", temp);
  printf("\n");
  MEMORY[addr >> 1][0] = temp & 0x00FF;
  MEMORY[addr >> 1][1] = (temp & 0xFF00) >> 8;

  NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
}

/* TRAP instruction */
void trap(int instr) {
  int vec = (instr & 0x00FF) << 1; /* the vector value is 0 extended and left shifted once */
  NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC + 2;
  NEXT_LATCHES.PC = (MEMORY[vec][0] & 0x000F)|((MEMORY[vec][1] & 0x000F) << 8);
}

void rti(int instr) {}
void nop(int instr) {}

