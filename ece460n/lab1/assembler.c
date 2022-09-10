//Name1: Seann Robbins
//Aniketh Deverasetty
//UTEID: snr2367
//UTEID: ad48322



// #include <stdbool.h>
#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

/////////////////////////////////////////////////////////////////////////////////////////
//                        THIS IS STARTER CODE : DO NOT EDIT                           //                                                                                         
/////////////////////////////////////////////////////////////////////////////////////////

#define MAX_LINE_LENGTH 255
int isOpcode(char* opcode);
	enum
	{
	   DONE, OK, EMPTY_LINE
	};

	int
	readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
	** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
	)
	{
	   char * lRet, * lPtr;
	   int i;
	   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
		    return( DONE );
	   for( i = 0; i < strlen( pLine ); i++ )
		pLine[i] = tolower( pLine[i] );
	   
           /* convert entire line to lowercase */
	   *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	   /* ignore the comments */
	   lPtr = pLine;

	   while( *lPtr != ';' && *lPtr != '\0' && *lPtr != '\n' ) 
		lPtr++;

	   *lPtr = '\0';
	   if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
		return( EMPTY_LINE );

	   if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
	   {
		*pLabel = lPtr;
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   }
	   
           *pOpcode = lPtr;

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   
           *pArg1 = lPtr;
	   
           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg2 = lPtr;
	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg3 = lPtr;

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg4 = lPtr;

	   return( OK );
	}

	/* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */

int
toNum( char * pStr )
{
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   { 
     pStr++;
     if( *pStr == '-' )				/* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
	    printf("Error: invalid decimal operand, %s\n",orig_pStr);
	    exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
     pStr++;
     if( *pStr == '-' )				/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
	printf( "Error: invalid operand, %s\n", orig_pStr);
	exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
//                                 END OF STARTER CODE                                 //                                                                                         
/////////////////////////////////////////////////////////////////////////////////////////

#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];	/* Question for the reader: Why do we need to add 1? For null end*/
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];
int symTableCount = 0;
    
int isOpcode(char* opcode)
    {
    if(strcmp(opcode, "add") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "and") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "halt") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "jmp") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "jsr") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "jsrr") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "ldb") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "ldw") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "lea") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "nop") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "not") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "ret") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "lshf") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "rshfl") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "rshfa") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "rti") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "stb") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "stw") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "trap") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "xor") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "brn") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "brp") == 0)
    {
        return 1;
    }
    else if(strcmp(opcode, "brnp") == 0)
    {
        return 1;
    }    
    else if(strcmp(opcode, "br") == 0)
    {
        return 1;
    } 
    else if(strcmp(opcode, "brz") == 0)
    {
        return 1;
    } 
    else if(strcmp(opcode, "brnz") == 0)
    {
        return 1;
    } 
    else if(strcmp(opcode, "brzp") == 0)
    {
        return 1;
    } 
    else if(strcmp(opcode, "brnzp") == 0)
    {
        return 1;
    } 
    return -1;
}

/*
    Functionality:
        This funtion is the first pass of the assembler. It will look for all labels in an assembly file and 
        assign them a memory address within a lookup table. This pass through will also determine where
        the beginning of the code will be placed in memory

    Non-Valid Labels : 
        IN, OUT, GETC, PUTS, .ORIG, .END, .FILL
        Cannot start with the letter 'x' 
        Cannot start with a number
        Cannot be the same as an opcode or pseudo-op

    Valid Labels:
        Consists of characters a-z or A-Z
        Consists of numbers 0-9

    Note:
        Labels are CASE-Sensitive
        Labels are necessary for branch operations
        Each line of assembly code, excluding solely commented lines, take up two bytes of memory

    Steps:
        1. Must look for the .ORIG to figure out where the starting address of the lookup table will be
        2. Find all labels within the document using readAndParse 
        3. Assign the label to a lookup table with its corresponding address
        4. Disregard

    ***THEY GIVE US A FUNCTION TO USE TO PARSE A LINE OF ASSEMBLY CODE***
        readAndParse returns 
            1. A Label with return code OK
            2. An op-code with return code OK
            3. Return code Done means line was neither of 1 or 2
            4. EMPTY_LINE 
*/
void passLabel(char* fil) {
    FILE* file = fopen(fil, "r");
    if (!file) {
        printf("Error: Cannot open pl file %s\n", fil);
        exit(4);
	}
    char line[MAX_LINE_LENGTH + 1];
    char* label = NULL;
    char* opcode;
    char* arg1;
    char* arg2;
    char* arg3;
    char* arg4;
    int status = readAndParse(file, line, &label, &opcode, &arg1, &arg2, &arg3, &arg4);
    int startAddr = 0x0;
    while (status != DONE)
    {
        // printf("Label: %s\nOpcode: %s\n", label, opcode);
        if(strcmp(opcode, ".orig") == 0)
        {
            // printf("Starting Address: %s\n", arg1);
            if (strcmp(arg1, "#"))
            {
                startAddr = toNum(arg1);
            }
            else
            {
                memmove(arg1, arg1+1, strlen(arg1));
                startAddr = strtol(arg1, NULL, 16);
            }
            //printf("Start Address: %x\n", startAddr);
            status = readAndParse(file, line, &label, &opcode, &arg1, &arg2, &arg3, &arg4);
            continue;
            // printf("Found starting address: %x\n", startAddr);
        }
        if (label[0] > 96 && label[0] < 123)
        {
            if (!strcmp(label, "getc"))
            {
                printf("Invalid Label Definition getc: %s\n", label);
                exit(-1);
            }
            else if (!strcmp(label, "puts"))
            {
                printf("Invalid Label Definition puts: %s\n", label);
                exit(-1);
            }
            else if (!strcmp(label, "in"))
            {
                printf("Invalid Label Definition in: %s\n", label);
                exit(-1);
            }
            else if (!strcmp(label, "out"))
            {
                printf("Invalid Label Definition out: %s\n", label);
                exit(-1);
            }
            //First letter is 'x'
            else if ((int)label[0] == 88)
            {
                printf("Invalid Label Definition x : %s\n", label);
                exit(-1);
            }
            // First letter is not a letter
            else if ((int)label[0] > 122 || (int)label[0] < 97) 
            {
                printf("Invalid Label Definition !char: %s, %s, %s, %s, %s, %s\n", line, label, opcode, arg1, arg2, arg3);
                exit(-1);
            }
            // IF LABELS CAN ONLY BE DEFINED ONCE UNCOMMENT
            // for (int i = 0; i < symTableCount; i++)
            // {
            //     if (!strcmp(symbolTable[i].label, label))
            //     {
            //         printf("Label defined twice: %s", label);
            //         exit(-1);
            //     }
            // }
            
        
            // printf("Found a label: %s\n", label);
            symbolTable[symTableCount].address = startAddr;
            // Copy over the label string to the symbolTable
            strcpy(symbolTable[symTableCount].label, label);
            //printf("Copied label: %s, %x\n", symbolTable[symTableCount].label, symbolTable[symTableCount].address);
            symTableCount++;
        }
        label = NULL;
        if(strcmp(opcode, "") != 0){
            startAddr += 0x02;
        }
        status = readAndParse(file, line, &label, &opcode, &arg1, &arg2, &arg3, &arg4);
    }
}

int branch(int n, int z, int p, int pc, char* label) {
    //I am assuming you can only branch to a label
    //pc = pc++ + LSHF(SEXT(pcoffset9), 1);
    int code = 0x0000;
    int tindex = 0;
    int offset;
    int machinecode;
    while (strcmp(symbolTable[tindex].label, label))
    {
        if (tindex >= symTableCount)
        {
            printf("Label not found in symbol table: %s\n", label);
            exit(-1);
        }       
        tindex++;
    }
    offset = (symbolTable[tindex].address - (pc + 0x02)) >> 1;
    //if (pc > symbolTable[tindex].address)
    //{
        offset = offset & 0x000001FF;
    //}
    machinecode = (code << 12) + (n << 11) + (z << 10) + (p << 9) + offset;
    machinecode = machinecode & 0x0000FFFF;
    return machinecode;
}


/*

    NOTE: We need to keep track of the pc 

*/
    
int secondPass(char* file, char* objfil)
{
    FILE* asmcode = fopen(file, "r");
    FILE* objfile = fopen(objfil, "w");

    if (!asmcode) {
       printf("Error: Cannot open assembly file %s\n", file);
       return(4);
	}
    if (!objfile) {
       printf("Error: Cannot open objfile %s\n", objfil);
       return(4);
    }

    char line[MAX_LINE_LENGTH + 1];
    char* label;
    char* opcode;
    char* arg1;
    char* arg2;
    char* arg3;
    char* arg4;
    static int pc;
    int status = readAndParse(asmcode, line, &label, &opcode, &arg1, &arg2, &arg3, &arg4);
    int machinecode;
    while (status != DONE)
    {  
        // printf("Found an instruction: %s, %s, %s, %s, %s, %s\n", label, opcode, arg1, arg2, arg3, arg4);
        if(opcode != NULL && strcmp(opcode, "") != 0)
        {
            if(strcmp(opcode, "add") == 0)
            {
                int code = 0x0001;
                *arg1++;
                int dr = atoi(arg1);
                *arg2++;
                int sr1 = atoi(arg2);
                machinecode = (code << 12) + (dr << 9) + (sr1 << 6);
                int sr2, addend;
                if(*arg3 == '#' || *arg3 == 'x')
                {
                    int num = 1;
                    machinecode += num << 5;
                    addend = toNum(arg3);
                    //if(addend < 0){
                        addend = addend & 0x0000001F;
                    //}
                    machinecode += addend;
                } else {
                    *arg3++;
                    sr2 = atoi(arg3);
                    machinecode += sr2;
                }
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "and") == 0)
            {
                int code = 0x5;
                *arg1++;
                int dr = atoi(arg1);
                *arg2++;
                int sr1 = atoi(arg2);
                machinecode = (code << 12) + (dr << 9) + (sr1 << 6);
                int sr2, addend;
                if(*arg3 == '#' || *arg3 == 'x')
                {
                    int num = 1;
                    machinecode += num << 5;
                    addend = toNum(arg3);
                    // if(addend < 0){
                        addend = addend & 0x0000001F;
                    // }
                    machinecode += addend; 
                } 
                else 
                {
                    *arg3++;
                    sr2 = atoi(arg3);
                    machinecode += sr2;
                }
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "halt") == 0)
            {
                int code = 0xF025;
                code = code & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", code);
            }
            else if(strcmp(opcode, "jmp") == 0)
            {
                int code = 0xC;
                *arg1++;
                int dr = atoi(arg1);
                machinecode = (code << 12) + (dr << 6);
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "jsr") == 0)
            {
                int code = 0x4;
                int addr = 0;
                int offset;
                if(*arg3 == '#' || *arg3 == 'x'){
                    offset = toNum(arg1);
                    // if(offset < 0){
                        offset = offset & 0x000007FF;
                    // }
                } 
                else {
                    for (int i = 0; i < MAX_SYMBOLS; i++)
                    {
                        if(strcmp(arg1, symbolTable[i].label) == 0)
                        {
                            addr = symbolTable[i].address;
                            break;
                        }
                    }
                    offset = (addr - (pc + 0x02)) >> 1;
                    // if (pc > addr)
                    // {
                        offset = offset & 0x000007FF;
                    // }
                }
               int one = 1;
               machinecode = (code << 12) + (one << 11) + offset;
               machinecode = machinecode & 0x0000FFFF;
               fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "jsrr") == 0)
            {
                int code = 0x4;
                *arg1++;
                int dr = atoi(arg1);
                machinecode = (code << 12) + (dr << 6);
                int x = 0;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "ldb") == 0)
            {
                int code = 0x2;
                *arg1++;
                int dr = atoi(arg1);
                *arg2++;
                int br = atoi(arg2);
                int offset = toNum(arg3);
                // if(offset < 0){
                    offset = offset & 0x0000003F;
                // }
                machinecode = (code << 12) + (dr << 9) + (br << 6) + offset;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "ldw") == 0)
            {
                int code = 0x6;
                *arg1++;
                int dr = atoi(arg1);
                *arg2++;
                int br = atoi(arg2);
                int offset = toNum(arg3);
                // if(offset < 0){
                    offset = offset & 0x0000003F;
                // }
                machinecode = (code << 12) + (dr << 9) + (br << 6) + offset;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "lea") == 0)
            {
                int code = 0xE;
                int addr = 0;
                *arg1++;
                int offset;
                // if(*arg2 == '#' || *arg2 == 'x'){
                //     offset = toNum(arg2);
                //     if(offset < 0){
                //         offset = offset & 0x000001FF;
                //     }
                // }  
                //else {
                    for (int i = 0; i < MAX_SYMBOLS; i++)
                    {
                        if(strcmp(arg2, symbolTable[i].label) == 0)
                        {
                            addr = symbolTable[i].address;
                            break;
                        }
                    }
                    offset = (addr - (pc + 0x02)) >> 1;
                    // if (pc > addr)
                    // {
                        offset = offset & 0x000001FF;
                    // }
                //}
                int dr = atoi(arg1);
                machinecode = (code << 12) + (dr << 9) + offset;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "nop") == 0)
            {
                int code = 0x0;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", code);
            }
            else if(strcmp(opcode, "not") == 0)
            {
                int code = 0x9;
                *arg1++;
                int dr = atoi(arg1);
                *arg2++;
                int sr = atoi(arg2);
                machinecode = (code << 12) + (dr << 9)+ (sr <<6) + 63;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "ret") == 0)
            {
                int code = 0xC1C0;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n",code);
            }
            else if(strcmp(opcode, "lshf") == 0)
            {
                int code = 0xD;
                *arg1++;
                int dr = atoi(arg1);
                *arg2++;
                int sr = atoi(arg2);
                int amount = toNum(arg3);
                machinecode = (code << 12) + (dr << 9) + (sr << 6) + amount;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "rshfl") == 0)
            {
                int code = 0xD;
                *arg1++;
                int dr = atoi(arg1);
                *arg2++;
                int sr = atoi(arg2);
                int amount = toNum(arg3);
                int one = 1;
                machinecode = (code << 12) + (dr << 9) + (sr << 6) + (one << 4) + amount;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "rshfa") == 0)
            {
                int code = 0xD;
                *arg1++;
                int dr = atoi(arg1);
                *arg2++;
                int sr = atoi(arg2);
                int amount = toNum(arg3);
                int one = 3;
                machinecode = (code << 12) + (dr << 9) + (sr << 6) + (one << 4)+ amount;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "rti") == 0)
            {
                int code = 0x8000;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", code);
            }
            else if(strcmp(opcode, "stb") == 0)
            {
                int code = 0x3;
                *arg1++;
                int sr = atoi(arg1);
                *arg2++;
                int br = atoi(arg2);
                int offset = toNum(arg3);
                // if(offset < 0){
                    offset = offset & 0x0000003F;
                // }
                machinecode = (code << 12) + (sr << 9) + (br << 6) + offset;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "stw") == 0)
            {
                int code = 0x7;
                *arg1++;
                int sr = atoi(arg1);
                *arg2++;
                int br = atoi(arg2);
                int offset = toNum(arg3);
                // if(offset < 0){
                    offset = offset & 0x0000003F;
                // }
                machinecode = (code << 12) + (sr << 9) + (br << 6) + offset;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "trap") == 0)
            {
                int code = 0xF;
                int vect = toNum(arg1);
                machinecode = (code << 12) + vect;
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "xor") == 0)
            {
                int code = 0x9;
                *arg1++;
                int dr = atoi(arg1);
                *arg2++;
                int sr1 = atoi(arg2);
                machinecode = (code << 12) + (dr << 9) + (sr1 << 6);
                int sr2, addend;
                if(*arg3 == '#' || *arg3== 'x')
                {
                    int num = 1;
                    machinecode += num << 5;
                    addend = toNum(arg3);
                    // if(addend < 0){
                        addend = addend & 0x0000001F;
                    // }
                    machinecode += addend;
                } else {
                    *arg3++;
                    sr2 = atoi(arg3);
                    machinecode += sr2;
                }
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "brn") == 0)
            {
                machinecode = branch(1, 0, 0, pc, arg1);
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "brp") == 0)
            {
                machinecode = branch(0, 0, 1, pc, arg1);
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, "brnp") == 0)
            {
                machinecode = branch(1, 0, 1, pc, arg1);
                fprintf(objfile, "0x%.4X\n", machinecode);
            }    
            else if(strcmp(opcode, "brz") == 0)
            {
                machinecode = branch(0, 1, 0, pc, arg1);
                fprintf(objfile, "0x%.4X\n", machinecode);
            } 
            else if(strcmp(opcode, "brnz") == 0)
            {
                machinecode = branch(1, 1, 0, pc, arg1);
                fprintf(objfile, "0x%.4X\n", machinecode);
            } 
            else if(strcmp(opcode, "brzp") == 0)
            {
                machinecode = branch(0, 1, 1, pc, arg1);
                fprintf(objfile, "0x%.4X\n", machinecode);
            } 
            else if(strcmp(opcode, "brnzp") == 0 || strcmp(opcode, "br") == 0)
            {
                machinecode = branch(1, 1, 1, pc, arg1);
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            else if(strcmp(opcode, ".orig") == 0)
            {
                if (strcmp(arg1, "#"))
                {
                    pc = toNum(arg1);
                }   
                else
                {
                    memmove(arg1, arg1+1, strlen(arg1));
                    pc = strtol(arg1, NULL, 16);
                }
                fprintf(objfile, "0x%.4X\n", pc);
                status = readAndParse(asmcode, line, &label, &opcode, &arg1, &arg2, &arg3, &arg4);
                continue;
            } 
            else if (strcmp(opcode, ".end") == 0)
            {
                return 0;
            }
            else if(strcmp(opcode, ".fill") == 0)
            {
                machinecode = toNum(arg1);
                machinecode = machinecode & 0x0000FFFF;
                fprintf(objfile, "0x%.4X\n", machinecode);
            }
            pc += 0x2;
        }
        status = readAndParse(asmcode, line, &label, &opcode, &arg1, &arg2, &arg3, &arg4);
    }
    fclose(asmcode);
    fclose(objfile);
}

int main(int argc, char* argv[])
{
    char* programName = argv[0];
    char* asmcode = argv[1];
    char* objcode = argv[2];



    passLabel(asmcode);
    secondPass(asmcode, objcode);
    // printf("symTableCount: %i\n", symTableCount);
    // for (int i = 0; i < symTableCount; i++)
    // {
    //     printf("Label %i: %s, %x\n", i, symbolTable[i].label, symbolTable[i].address);
    // }
    
    // printf("%x\n", secondPass(asmcode, objcode));
    
    //  fclose(asmcode);
    //  fclose(objcode);

    return 0;
}