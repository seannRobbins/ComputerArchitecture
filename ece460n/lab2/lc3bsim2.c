/*
Name1: Seann Robbins
Aniketh Deverasetty
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

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
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

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *files[], int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(files[i]);
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

  initialize(&argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/*
   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

/*
    Need to copy over or reset all non-changed latches from Current to Next
*/

void nzpCopy(){
    //Assuming we copy nzp, may clear? idk
    NEXT_LATCHES.N = CURRENT_LATCHES.N;
    NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
    NEXT_LATCHES.P = CURRENT_LATCHES.P;
}

void regCopy(){
    for (int i = 0; i < LC_3b_REGS; i++)
    {
      NEXT_LATCHES.REGS[i] = CURRENT_LATCHES.REGS[i];
    }
}

void incrPC(){
    NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 0x02);
}

int getBits(int instruction, int start, int end){
    int mask = 0;
    for(int i = start; i <= end; i++){
        mask += 1 << i;
    }
    return Low16bits((instruction & mask) >> start);
    //return Low16bits(mask);
}

void setnzp(int val){
    if (!val) //zero
    {
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
    else if (val & 0x8000) //neg
    {
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }
    else  //pos
    {
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;
    }
}

void brHelp(int status, int instruction)
{
    int offset = 0;
    int pc = CURRENT_LATCHES.PC;
    if (status) //We do the branch
    {
        offset = getBits(instruction, 0, 8); //1ff
        if (offset & 0x100) //neg offset
        {
            offset |= 0xFE00; //sign extend
        }
        offset = Low16bits(offset << 1);
    }
    
    NEXT_LATCHES.PC = Low16bits(pc + 0x02 + offset);
}

void br(int instruction)
{
    int nzp = getBits(instruction, 9, 11);
    int N = CURRENT_LATCHES.N;
    int Z = CURRENT_LATCHES.Z;
    int P = CURRENT_LATCHES.P;

    switch (nzp)
    {
    case 0:
        if (~N && ~Z && ~P)
        {
            brHelp(1, instruction);
        }
        else
        {
            brHelp(0, instruction);
        }
    case 1:
        if (P)
        {
            brHelp(1, instruction);
        }
        else
        {
            brHelp(0, instruction);
        }
    case 2:
        if (Z)
        {
            brHelp(1, instruction);
        }
        else
        {
            brHelp(0, instruction);
        }
    case 3:
        if (Z && P)
        {
            brHelp(1, instruction);
        }
        else
        {
            brHelp(0, instruction);
        }
    case 4:
        if (N)
        {
            brHelp(1, instruction);
        }
        else
        {
            brHelp(0, instruction);
        }
    case 5:
        if (N && P)
        {
            brHelp(1, instruction);
        }
        else
        {
            brHelp(0, instruction);
        }
    case 6:
        if (N && Z)
        {
            brHelp(1, instruction);
        }
        else
        {
            brHelp(0, instruction);
        }
    case 7:
        if (N && Z && P)
        {
            brHelp(1, instruction);
        }
        else
        {
            brHelp(0, instruction);
        }
    default:
        brHelp(0, instruction);
        break;
    }
    regCopy();
    //Assuming we copy nzp, may clear? idk
    nzpCopy();
}

void add(int instruction)
{
    int bit5 = getBits(instruction, 5, 5); //x20
    int dr = getBits(instruction, 9, 11); //e00
    int sr1 = getBits(instruction, 6, 8); //1c0
    int sr2;
    int val1 = Low16bits(CURRENT_LATCHES.REGS[sr1]); //registers are 16-bits
    int val2;
    int val;
    
    if (bit5) //pulling from imm5
    {
        val2 = getBits(instruction, 0, 4); //1f
        if (val2 & 0x10)
        {
            val2 |= 0xFFE0; //negative number, sign extend
        }
    }
    else //pulling from another register
    {
        sr2 = getBits(instruction, 0, 2); //7
        val2 = Low16bits(CURRENT_LATCHES.REGS[sr2]); //registers are 16-bits
    }

    val = Low16bits(val1 + val2); // Not sure if this is right *****************

    setnzp(val);
    regCopy();
    NEXT_LATCHES.REGS[dr] = val;
    
}

void ldb(int instruction)
{
    int dr = getBits(instruction, 9, 11);
    int br = getBits(instruction, 6, 8); //1c0
    int off1 = getBits(instruction, 0, 5); //3f
    int off2 = Low16bits(CURRENT_LATCHES.REGS[br]); // value in br 
    int val;
    if (off1 & 0x20) // neg
    {
        off1 |= 0xFFC0; //sign extend 16 bits
    }
    int location = Low16bits(off1 + off2);
    int bit = location & 0x0001;
    location = location >> 1;
    val = MEMORY[location][bit];
    if (val & 0x80)
    {
        val |= 0xFF80; //sign extend
    }
    setnzp(val);
    regCopy();
    NEXT_LATCHES.REGS[dr] = val;
}

void stb(int instruction)
{
    int sr = getBits(instruction, 9, 11); //e00
    int br = getBits(instruction, 6, 8); //1c0
    int off1 = getBits(instruction, 0, 5); //3f
    int off2 = Low16bits(CURRENT_LATCHES.REGS[br]); // value in br 
    int val = Low16bits(CURRENT_LATCHES.REGS[sr]) & 0x0FF; //sr[7:0]
    int location = Low16bits(off1 + off2);
    int bit = location & 0x0001;
    location = location >> 1;
    MEMORY[location][bit] = val;
    regCopy();
    //Assuming we copy nzp, may clear? idk
    nzpCopy();
}

void jsr_jsrr(int instruction)
{
    nzpCopy();
    regCopy();
    NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC + 0x02;
    int type = getBits(instruction, 11, 11); //400
    if(type == 0){
      int br = getBits(instruction, 6, 8);
      NEXT_LATCHES.PC = br;
    } else {
      int offset = getBits(instruction, 0, 10); //7ff
      if (offset & 0x400) //neg offset
      {
          offset |= 0xF800; //sign extend
      }
      offset = Low16bits(offset << 1);
      NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 0x02 + offset);
    }
}

void and(int instruction)
{
    int bit5 = getBits(instruction, 5, 5); //x20
    int dr = getBits(instruction, 9, 11); //e00
    int sr1 = getBits(instruction, 6, 8); //1c0
    int bit4 = getBits(instruction, 4, 4); //10
    int sr2;
    int val1 = Low16bits(CURRENT_LATCHES.REGS[sr1]); //registers are 16-bits
    int val2;
    int val;
    
    if (bit5) //pulling from imm5
    {
        val2 = getBits(instruction, 0, 4); //1f
        if (bit4)
        {
            val2 |= 0xFFE0; //negative number, sign extend
        }
    }
    else //pulling from another register
    {
        sr2 = getBits(instruction, 0, 2); //7
        val2 = Low16bits(CURRENT_LATCHES.REGS[sr2]); //registers are 16-bits
    }

    val = Low16bits(val1 & val2); // Not sure if this is right *****************

    setnzp(val);
    regCopy();
    NEXT_LATCHES.REGS[dr] = val;
}

void ldw(int instruction)
{
    int dr = getBits(instruction, 9, 11); //e00
    int br = getBits(instruction, 6, 8); //1c0
    int off1 = getBits(instruction, 0, 5); //3f
    int off2 = Low16bits(CURRENT_LATCHES.REGS[br]); // value in br 
    int val;
    if (off1 & 0x20) //neg
    {
      off1 |= 0xFFC0; //sign extend
    }
    off1 = Low16bits(off1 << 1);
    int location = Low16bits(off1 + off2);
    location = location >> 1;
    val = (MEMORY[location][1] << 8) + MEMORY[location][0]; // MSB + LSB
    setnzp(val);
    regCopy();
    NEXT_LATCHES.REGS[dr] = val;
     
}

void stw(int instruction)
{
    int sr = getBits(instruction, 9, 11); //e00
    int br = getBits(instruction, 6, 8); //1c0
    int off1 = getBits(instruction, 0, 5); //3f
    int off2 = Low16bits(CURRENT_LATCHES.REGS[br]); // value in br 
    int val = Low16bits(CURRENT_LATCHES.REGS[sr]); //sr[15:0]
    if (off1 & 0x20) //neg
    {
      off1 |= 0xFFC0; //sign extend
    }
    off1 = Low16bits(off1 << 1);
    int location = Low16bits(off1 + off2);
    location = location >> 1;
    MEMORY[location][0] = val & 0x00FF; //sr[7:0]
    MEMORY[location][1] = (val & 0xFF00) >> 8; //sr[15:8]
    regCopy();
    //Assuming we copy nzp, may clear? idk
    nzpCopy();
}

void rti(int instruction)
{
    return;
}

// void not(int instruction)
// {
//     int dr = (instruction & 0xE00) >> 9;
//     int sr = (instruction & 0x1C0) >> 6;
//     int val = Low16bits(CURRENT_LATCHES.REGS[sr]);
//     val = Low16bits(~val);
//     if (!val) //neg
//     {
//       NEXT_LATCHES.N = 0;
//       NEXT_LATCHES.Z = 1;
//       NEXT_LATCHES.P = 0;
//     }
//     else if (val & 0x8000) //neg
//     {
//       NEXT_LATCHES.N = 1;
//       NEXT_LATCHES.Z = 0;
//       NEXT_LATCHES.P = 0;
//     }
//     else //pos
//     {
//       NEXT_LATCHES.N = 0;
//       NEXT_LATCHES.Z = 0;
//       NEXT_LATCHES.P = 1;
//     }
//     regCopy();
//     NEXT_LATCHES.REGS[dr] = val;
//     incrPC();
// }

void jmp_ret(int instruction)
{
    int mode = (instruction & 0x0380) >> 6;
    NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[mode];
    // if(mode == 7){ //return
    //     NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[7];
    // } else {
    //     NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[mode];
    // }
    regCopy();
    nzpCopy();
}

int lshf(int dr, int sr, int bit)
{
    return Low16bits(CURRENT_LATCHES.REGS[sr] << bit);
}

int rshfl(int dr, int sr, int bit)
{
    return Low16bits(CURRENT_LATCHES.REGS[sr]) >> bit;
}

int rshfa(int dr, int sr, int bit)
{
    int msb = 0x01 << bit;
    int invBit = 16 - bit;
    msb = (CURRENT_LATCHES.REGS[sr] & msb) << invBit; 
    return (Low16bits(CURRENT_LATCHES.REGS[sr]) >> bit) + msb;
}

void shf(int instruction)
{
    int direction = getBits(instruction, 4, 5); //30
    int dr = getBits(instruction, 9, 11);//e00
    int sr = getBits(instruction, 6, 8); //1c0
    int bit = getBits(instruction, 0, 3); //f
    int val = 0;
    switch (direction)
    {
    case 0:
      val = lshf(dr, sr, bit);
      break;
    case 1:
       val = rshfl(dr, sr, bit);
      break;
    case 3:
      val = rshfa(dr, sr, bit);
      break;
    default:
      break;
    }
    setnzp(val);
    regCopy();
    NEXT_LATCHES.REGS[dr] = val;
}

void not_xor(int instruction){
    int mode = getBits(instruction, 5, 5); //20
    int dr = getBits(instruction, 9, 11);
    int sr = getBits(instruction, 6, 8); //1c0
    int val = Low16bits(CURRENT_LATCHES.REGS[sr]);
    int val2;
    int solution;
    if(mode){ //IMMEDIATE
        val2 = getBits(instruction, 0, 4); //1f
    } else { //REGISTER
        int sr2 = getBits(instruction, 0, 2); //3
        val2 = Low16bits(CURRENT_LATCHES.REGS[sr2]);
    }
    solution = val ^ val2;
    if (solution & 0x10) //neg
    {
      solution |= 0xFFE0; //sign extend
    }
    regCopy();
    setnzp(solution);
    NEXT_LATCHES.REGS[dr] = solution;
}

void lea(int instruction)
{
    int offset = getBits(instruction, 0, 8); //1ff
    if (offset & 0x100) //neg offset
    {
        offset |= 0xFE00; //sign extend
    }
    offset = Low16bits(offset << 1);
    int dr = getBits(instruction, 9, 11); //e00
    int location = Low16bits(CURRENT_LATCHES.PC + 0x02 + offset) >> 1;
    int num = MEMORY[location][0] + (MEMORY[location][1] << 8);
    regCopy();
    NEXT_LATCHES.REGS[dr] = num;
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 0x02;
    //not changing lea
    nzpCopy();
}

void trap(int instruction)
{
    int vector = getBits(instruction, 0, 7); //ff
    vector = Low16bits(vector & 0x00FF);
    int r7 = 7;
    vector = Low16bits(vector << 1);
    regCopy();
    NEXT_LATCHES.REGS[r7] = Low16bits(CURRENT_LATCHES.PC + 0x02);
    vector = Low16bits(vector >> 1);
    NEXT_LATCHES.PC = (MEMORY[vector][1] << 8) + MEMORY[vector][0];
    //Assuming we copy nzp, may clear? idk
    nzpCopy();
}

void decode(int instruction) 
{
    int opcode = getBits(instruction, 12, 15); //f000
    switch (opcode)
    {
    case 0:
        br(instruction);
        break;
    case 1:
        incrPC();
        add(instruction);
        break;
    case 2:
        incrPC();
        ldb(instruction);
        break;
    case 3:
        incrPC();
        stb(instruction);
        break;
    case 4:
        jsr_jsrr(instruction);
        break;
    case 5:
        incrPC();
        and(instruction);
        break;
    case 6:
        incrPC();
        ldw(instruction);
        break;
    case 7:
        incrPC();
        stw(instruction);
        break;
    case 8:
        rti(instruction);
        break;
    case 9:
        incrPC();
        not_xor(instruction);
        break;
    case 12:
        jmp_ret(instruction);
        break;
    case 13:
        incrPC();
        shf(instruction);
        break;
    case 14:
        incrPC();
        lea(instruction);
        break;
    case 15:
        trap(instruction);
        break;
    
    default:
        break;
    }
}

void process_instruction()
{
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */     
    int pc = CURRENT_LATCHES.PC;
    int location = pc >> 1;
    int instruction = (MEMORY[location][1] << 8) + MEMORY[location][0];
    decode(instruction);
}