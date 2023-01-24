/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

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
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX,
    SR1MUX,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    LD_VEC,
    LD_USP,
    LD_SSP,
    LD_PSR,
    GATE_SP,
    GATE_PSR,
    GATE_VEC,
    SPMUX1, SPMUX0,
    PSRMUX,
    SR1MUX1,
    DRMUX1,
    COND2,
/* MODIFY: you have to add all your new control signals */
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }
int GetLD_VEC(int *x)         { return(x[LD_VEC]); }
int GetLD_USP(int *x)         { return(x[LD_USP]); }
int GetLD_SSP(int *x)         { return(x[LD_SSP]); }
int GetLD_PSR(int *x)         { return(x[LD_PSR]); }
int GetGATE_SP(int *x)         { return(x[GATE_SP]); }
int GetGATE_PSR(int *x)         { return(x[GATE_PSR]); }
int GetGATE_VEC(int *x)         { return(x[GATE_VEC]); }
int GetSPMUX(int *x)         { return((x[SPMUX1] << 1) + x[SPMUX0]); }
int GetPSRMUX(int *x)         { return(x[PSRMUX]); }
/* MODIFY: you can add more Get functions for your new control signals */

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x08000 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    PSR_15, /* Privilege Bit */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;    /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* Initial value of system stack pointer */
int USP; /* Initial value of user stack pointer */
/* MODIFY: You may add system latches that are required by your implementation */

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  eval_micro_sequencer();
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();
  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
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
/* Purpose   : Simulate the LC-3b until HALTed.                 */
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

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
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
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
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

    switch(buffer[0]) {
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
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
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

    for (i=0; i < WORDS_IN_MEM; i++) {
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

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 2]);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */
    CURRENT_LATCHES.USP = 0xFE00; /* Initial value of system stack pointer */
    CURRENT_LATCHES.PSR_15 = 1;     /* Intialize to user mode */
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
    if (argc < 3) {
	printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 2);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.

   Do not modify the rdump and mdump functions.

   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/


/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/
#define INTERRUPT_EXCEPTION_START_STATE 26
#define TRAP_MEMORY_ACCESS_STATE 15
int mem_count = 0;
int marmux = 0;
int alu = 0;
int shf = 0;
int mdr = 0;
int ie = FALSE;
int interrupt = FALSE;
int interrupt_enable = FALSE;
int exception_enable = FALSE;
int vec = 0;

int lshf(int val, int amount)
{
    return Low16bits(val << amount);
}

int rshfa(int val, int amount)
{
    return Low16bits(((int16_t) val) >> amount);
}

int rshfl(int val, int amount)
{
    return Low16bits(((unsigned int) val) >> amount);
}

int sign_extend(int val, int endBit) 
{

    // xxxxxxxxxx 000000
    int sign_extension = 0;
    for (int i = 0; i <= endBit; i++)
    {
        sign_extension = sign_extension << 1;
        sign_extension += 0x01;
    }

    sign_extension = Low16bits(~sign_extension);

    endBit = 0x01 << endBit;   
    if (val & endBit)
    {
        val |= sign_extension;
    }
    return val;
}

void eval_micro_sequencer() {

  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
   * 
   * Check IRD - if yes Next State is 00IR[15:12]
   * Check Cond - 00 nothing, 01 memory R-bit << 1, 10 Branch BEN << 2, 11 IR[11]  
   * Check J
   * 
   * 
   */

    int ird = GetIRD(CURRENT_LATCHES.MICROINSTRUCTION);
    int cond = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION);
    int j = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
    int next_state = j;
    
    // Check for interrupt
    if (CYCLE_COUNT == 300)
    {
        interrupt = TRUE;
    }
    // Wait until end of current instruction
    if (interrupt && (CURRENT_LATCHES.STATE_NUMBER == 18 || CURRENT_LATCHES.STATE_NUMBER == 19))
    {
        interrupt = FALSE;
        ie = TRUE;
        interrupt_enable = TRUE;
        vec = 0x01;
        NEXT_LATCHES.INTV = 0x01;
    }
    
    if (ird == 1) 
    {
        next_state = rshfl(CURRENT_LATCHES.IR, 12) & 0x0F;

        // Check for unknown opcode exception
        if (next_state == 10 || next_state == 11)
        {
            vec = 0x04;
            NEXT_LATCHES.EXCV = 0x04;
            exception_enable = TRUE;
            next_state = INTERRUPT_EXCEPTION_START_STATE;
            // printf("Vector: %x\n", vec);
        }
    }
    else
    {
        if (ie == TRUE)
        {
            ie = FALSE;
            next_state = INTERRUPT_EXCEPTION_START_STATE;
            // printf("Vector: %x\n", vec);
        }
        else
        {
            switch (cond)
            {
            // Conditioned on Memory Ready
            case 1:
                next_state |= lshf(CURRENT_LATCHES.READY, 1);
                NEXT_LATCHES.READY = 0;
                break;

            // Conditioned on Branch Enabled
            case 2:
                next_state |= lshf(CURRENT_LATCHES.BEN, 2);
                break;

            // Conditioned on Addressing Mode
            case 3:
                next_state |= rshfl(CURRENT_LATCHES.IR & 0x0800, 11);
                break;

            // Conditioned on PSR[15]
            case 4:
                next_state |= lshf(CURRENT_LATCHES.PSR_15, 3);
                break;
            
            default:
                break;
            }
        }
    }
    memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[next_state], sizeof(int)*CONTROL_STORE_BITS);
    NEXT_LATCHES.STATE_NUMBER = next_state;
    // printf("Current State: %i\n", CURRENT_LATCHES.STATE_NUMBER);
    // printf("Next State: %i\n", next_state);
}

void cycle_memory() {
 
  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle. 
   * 
   * 
   *  
   */

    //If memory access is enabled
    if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION))
    {  
        if (mem_count == 3)
        {
            NEXT_LATCHES.READY = 1;
            // printf("Memory Ready: %i\n", NEXT_LATCHES.READY);
            mem_count++;
        }
        else if (mem_count == 4)
        {
            mem_count = 0;
        }
        else
        {
            mem_count++;
        }
        // printf("Memory Count: %i\n", mem_count);
    }
}

int eval_addr1()
{
    int retVal;

    if (GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION))
        {
            retVal = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x01C0) >> 6];
        } 
        else
        {
            retVal = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x0E00) >> 9];
        }
    }
    else
    {
        retVal = CURRENT_LATCHES.PC;
    }

    return retVal;
}

int eval_addr2()
{
    int retVal;

    switch (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION))
    {
    case 0:
        retVal = 0;
        break;

    case 1:
        retVal = CURRENT_LATCHES.IR & 0x03F; // IR[5:0]
        if (CURRENT_LATCHES.IR & 0x020) // sign extend if bit 5 is 1
        {
            retVal = sign_extend(retVal, 5);
        }
        break;

    case 2:
        retVal = CURRENT_LATCHES.IR & 0x01FF; // IR[8:0]
        if (CURRENT_LATCHES.IR & 0x0100) // sign extend if bit 8 is 1
        {
            retVal = sign_extend(retVal, 8);
        }
        break;

    case 3:
        retVal = CURRENT_LATCHES.IR & 0x07FF; // IR[10:0]
        if (CURRENT_LATCHES.IR & 0x0400) // sign extend if bit 10
        {
            retVal = sign_extend(retVal, 10); 
        }
        break;

    default:
        break;
    }

    if (GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        retVal = retVal << 1;
    }

    return retVal;
}

void gate_marmux() {

    //Output of the ADDER
    if (GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        marmux = Low16bits(eval_addr1() +  eval_addr2());
    }
    //Output of operations on IR[7:0]
    else
    {
        marmux = Low16bits((CURRENT_LATCHES.IR & 0x0FF) << 1);
    }
}

int add(int val1, int val2)
{
    return Low16bits(val1 + val2);
}

int and(int val1, int val2)
{
    return Low16bits(val1 & val2);
}

int xor(int val1, int val2)
{
    return Low16bits(val1 ^ val2);
}

void gate_alu() {
    int val1;
    int val2;
    int retVal;

    switch (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION))
    {
    //val1 is contents of register defined by IR[11:9]
    case 0:
        val1 = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x0E00) >> 9]; 
        break;

    // val1 is contents of register defined by IR[8:6]
    case 1:
        val1 = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x01C0) >> 6]; 
        break;

    // val1 is contents of register 6 (SP)
    case 2:
        val1 = CURRENT_LATCHES.REGS[6]; 
    
    default:
        break;
    }

    // check bit 5 of IR: 1 IR[4:0], 0 contents of register defined by IR[2:0]
    if (CURRENT_LATCHES.IR & 0x020)
    {
        val2 = sign_extend(CURRENT_LATCHES.IR & 0x01F, 4);
    }
    else
    {
        val2 = CURRENT_LATCHES.REGS[CURRENT_LATCHES.IR & 0x07];
    }

    switch (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION))
    {
    case 0:
        alu = add(val1, val2);
        break;

    case 1:
        alu = and(val1, val2);
        break;

    case 2:
        alu = xor(val1, val2);
        break;

    case 3:
        alu = val1;
        break;
    
    default:
        alu = 0;
        break;
    }
}

void gate_shf() {
    int val1;
    int tbits = (CURRENT_LATCHES.IR & 0x030) >> 4;
    int amount = (CURRENT_LATCHES.IR & 0x0F);
    // 1: contents of register defined by IR[8:6]
    if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        val1 = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x01C0) >> 6];
    }
    // 0: contents of register defined by IR[11:9]
    else
    {
        val1 = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x0E00) >> 6]; 
    }
    
    switch (tbits)
    {
    case 0:
        shf = lshf(val1, amount);
        break;

    case 1:
        shf = rshfl(val1, amount);
        break;

    case 3:
        shf = rshfa(val1, amount);
        break;
    
    default:
        break;
    }
}

void gate_mdr() {
    /*
        Value depends on
        MAR[0]: 0 MDR[7:0], 1 MDR[15:8] when data size is byte
        DATA_SIZE: 0 byte, 1 word
    */

    // data size is word
    if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        mdr = CURRENT_LATCHES.MDR;
    }
    // data size is byte
    else
    {
        // MAR[0] == 1
        if (CURRENT_LATCHES.MAR & 0x01)
        {
            mdr = rshfa((CURRENT_LATCHES.MDR & 0xFF00), 8);
        }
        // MAR[0] == 0
        else
        {
            mdr = (CURRENT_LATCHES.MDR & 0x00FF);
        }
    }
}

void eval_bus_drivers() {

  /* 
   * Datapath routine emulating operations before driving the bus.
   * Evaluate the input of tristate drivers 
   *         Gate_MARMUX,
   *		 Gate_PC,
   *		 Gate_ALU,
   *		 Gate_SHF,
   *		 Gate_MDR.
   */    
    gate_marmux();
    gate_alu();
    gate_shf();
    gate_mdr();
}

void drive_bus() {

  /* 
   * Datapath routine for driving the bus from one of the 5 possible 
   * tristate drivers. 
   */       

    if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = Low16bits(alu);
    }
    else if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = Low16bits(marmux);
    }
    else if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = Low16bits(mdr);
    }
    else if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = Low16bits(CURRENT_LATCHES.PC);
    }
    else if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = Low16bits(shf);
    }
    else if (GetGATE_PSR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        BUS = lshf(CURRENT_LATCHES.PSR_15, 3) + lshf(CURRENT_LATCHES.N, 2) + lshf(CURRENT_LATCHES.Z, 1) + CURRENT_LATCHES.P;
    }
    else if (GetGATE_SP(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        switch (GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION))
        {
        case 0:
            BUS = CURRENT_LATCHES.USP;
            break;

        case 1:
            BUS = CURRENT_LATCHES.REGS[6] + 0x02; 
            break;

        case 2:
            BUS = CURRENT_LATCHES.REGS[6] - 0x02; 
            break;

        case 3:
            BUS = CURRENT_LATCHES.SSP;
        
        default:
            break;
        }
    }
    else if (GetGATE_VEC(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        if (interrupt_enable == TRUE)
        {
            interrupt_enable = FALSE;
            BUS = 0x0200 + lshf(CURRENT_LATCHES.INTV, 1);
        }
        else if (exception_enable == TRUE)
        {
            exception_enable = FALSE;
            BUS = 0x0200 + lshf(CURRENT_LATCHES.EXCV, 1);
        }
    }
    else
    {
        BUS = 0;
    }
    // printf("Bus: %x\n", BUS);
}

void latch_ben()
{

    /*
        nzp ----> nzp
        bbits --> abc

        nzp & abc will only be true if (n & a) + (z & b) + (p & c) is not zero
    */

    int nzp = (CURRENT_LATCHES.N << 2) + (CURRENT_LATCHES.Z << 1) + CURRENT_LATCHES.P;
    int bbits = (CURRENT_LATCHES.IR & 0x0E00) >> 9;
    if (nzp & bbits)
    {
        NEXT_LATCHES.BEN = 1;
    }
    else
    {
        NEXT_LATCHES.BEN = 0;
    }
}

void latch_cc()
{
    if (!BUS)
    {
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
    else if (BUS & 0x8000)
    {
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }
    else
    {
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;
    }
}

void latch_pc()
{
    // printf("In latch PC: %i\n", GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION));
    switch (GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION))
    {
    case 0:
        NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 0x02);
        break;

    case 1:
        NEXT_LATCHES.PC = Low16bits(BUS);
        break;

    case 2:
        NEXT_LATCHES.PC = Low16bits(eval_addr1() + eval_addr2());
        break;

    case 3:
        NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC - 0x02);
        break;

    default:
        break;
    }
}

void latch_ir()
{
    NEXT_LATCHES.IR = BUS;
}

int protection_exception_check(int MAR)
{
    int bit15 = rshfl(MAR & 0x8000, 15);
    int bit14 = rshfl(MAR & 0x4000, 14);
    int bit13 = rshfl(MAR & 0x2000, 13);
    int bit12 = rshfl(MAR & 0x1000, 12);
    
    // MAR is greater than 0x3000
    if ((bit13 & bit12) == 1 || bit14 == 1 || bit15 == 1)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

int unaligned_exception_check(int MAR)
{
    // Check if data size is Word, and MAR[0] == 1
    // printf("Unaligned Except Data Size: %x\n", GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION));
    // printf("Unaligned Except MAR: %x\n", MAR);
    if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION) == 1 && MAR & 0x01 == 1)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void latch_mar()
{
    NEXT_LATCHES.MAR = BUS;
    if (CURRENT_LATCHES.STATE_NUMBER != TRAP_MEMORY_ACCESS_STATE)
    {
        // Check for protection exception
        if (protection_exception_check(BUS) == TRUE && CURRENT_LATCHES.PSR_15 == 1)
        {
            ie = TRUE;
            vec = 0x02;
            NEXT_LATCHES.EXCV = 0x02;
            exception_enable = TRUE;
            return;
        }

        // Check for unaligned exception
        else if (unaligned_exception_check(BUS) == TRUE)
        {   
            ie = TRUE;
            vec = 0x03;
            NEXT_LATCHES.EXCV = 0x03;
            exception_enable = TRUE;
            return;
        }
    }
}

void latch_mdr()
{   
    // Latching from memory
    if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        // MDR always gets all 16 bits from memory
        NEXT_LATCHES.MDR = (MEMORY[(rshfl(Low16bits(CURRENT_LATCHES.MAR), 1))][1] << 8) + MEMORY[rshfl(Low16bits(CURRENT_LATCHES.MAR), 1)][0]; 
    }
    // Latching from BUS
    else
    {
        // Word of data
        if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
        {
            NEXT_LATCHES.MDR = BUS;
        }
        // Byte of data
        else
        {
            // Store SR[7:0]'SR[7:0] into MDR i.e MDR[15:8] == MDR[7:0]
            NEXT_LATCHES.MDR = ((BUS & 0x00FF) << 8) + (BUS & 0x00FF);
        }
    }
}

void latch_reg()
{
    switch (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION))
    {
    // destination is register defined by IR[11:9]
    case 0:
        NEXT_LATCHES.REGS[(CURRENT_LATCHES.IR & 0x0E00) >> 9] = BUS;
        break;
    
    // destination is R7
    case 1:
        NEXT_LATCHES.REGS[7] = BUS;
        break;

    // destination is R6
    case 2:
        NEXT_LATCHES.REGS[6] = BUS;
        break;
    
    default:
        break;
    }
}

void latch_mem()
{
    int location = CURRENT_LATCHES.MAR >> 1;
    // Data is a Word
    if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        MEMORY[location][0] = CURRENT_LATCHES.MDR & 0x00FF;
        MEMORY[location][1] = (CURRENT_LATCHES.MDR & 0xFF00) >> 8;
    }
    // Data is a Byte
    else
    {
        // if MAR[0] == 1 store MDR[15:8] into MEM[1]
        // if MAR[0] == 0 store MDR[7:0] into MEM[0]
        // MAR[0] == 1
        if (CURRENT_LATCHES.MAR & 0x01)
        {
            MEMORY[location][CURRENT_LATCHES.MAR & 0x01] = (CURRENT_LATCHES.MDR & 0xFF00) >> 8;
        }
        // MAR[0] == 0
        else
        {
            MEMORY[location][CURRENT_LATCHES.MAR & 0x01] = (CURRENT_LATCHES.MDR & 0x00FF);
        }
    }
}

void latch_psr()
{
    int psr;
    switch (GetPSRMUX(CURRENT_LATCHES.MICROINSTRUCTION))
    {
    case 0:
        psr = BUS;
        NEXT_LATCHES.PSR_15 = rshfl(psr & 0x08, 3);
        NEXT_LATCHES.N = rshfl(psr & 0x04, 2);
        NEXT_LATCHES.Z = rshfl(psr & 0x02, 1);
        NEXT_LATCHES.P = psr & 0x01;
        break;

    case 1:
        NEXT_LATCHES.PSR_15 = 0;
        NEXT_LATCHES.N = CURRENT_LATCHES.N;
        NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
        NEXT_LATCHES.P = CURRENT_LATCHES.P;
    
    default:
        break;
    }
}

void latch_ssp()
{
    NEXT_LATCHES.SSP = CURRENT_LATCHES.REGS[6];
}

void latch_usp()
{
    NEXT_LATCHES.USP = CURRENT_LATCHES.REGS[6];
}

void latch_datapath_values() 
{
    /* 
   * Datapath routine for computing all functions that need to latch
   * values in the data path at the end of this cycle.  Some values
   * require sourcing the bus; therefore, this routine has to come 
   * after drive_bus.
   * 
   * Values to latch
   * MDR
   * MAR
   * IR
   * PC
   * BEN
   * REG
   * CC
   * memory
   */       
    if (GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        latch_ben();
    }
    if (GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        latch_cc();
    }
    if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        latch_pc();
    }
    if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        latch_ir();
    }
    if (GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        latch_mar();
    }
    if (GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        latch_mdr();
    }
    if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        latch_reg();
    }
    if (GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        latch_psr();
    }
    if (GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        latch_ssp();
    }
    if (GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        latch_usp();
    }
    if (GetR_W(CURRENT_LATCHES.MICROINSTRUCTION))
    {
        latch_mem();
    }
}