void passLabel(char* fil) {
    FILE* file = fopen(fil, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", fil);
        exit(4);
	}
    char line[MAX_LINE_LENGTH + 1];
    char* label = NULL;
    char* opcode;
    char* arg1;
    char* arg2;
    char* arg3;
    char* arg4;
    int index = 0;
    int status = readAndParse(file, line, &label, &opcode, &arg1, &arg2, &arg3, &arg4);
    int startAddr = 0x0;
    while (status != DONE)
    {
        printf("Label: %s\nOpcode: %s\n", label, opcode);
        if(strcmp(opcode, ".orig") == 0){
            printf("Starting Address: %s\n", arg1);
            startAddr = strtol(arg1, NULL, );
            printf("Found starting address: %i\n", startAddr);

        }
        if (strcmp(label, "\0"))
        {
            if (!strcmp(label, "getc")){
                printf("Invalid Label Definition: %s\n", label);
                exit(-1);
            }
            else if (!strcmp(label, "puts")){
                printf("Invalid Label Definition: %s\n", label);
                exit(-1);
            }
            else if (!strcmp(label, "in")){
                printf("Invalid Label Definition: %s\n", label);
                exit(-1);
            }
            else if (!strcmp(label, "out")){
                printf("Invalid Label Definition: %s\n", label);
                exit(-1);
            }
            //First letter is 'x'
            else if ((int)label[0] == 88)
            {
                printf("Invalid Label Definition: %s\n", label);
                exit(-1);
            }
            // First letter is not a letter
            else if ((int)label[0] > 122 || (int)label[0] < 97) 
            {
                printf("Invalid Label Definition: %s\n", label);
                exit(-1);
            }
        
            printf("Found a label: %s\n", label);
            symbolTable[index].address = startAddr;
            // Copy over the label string to the symbolTable
            strcpy(symbolTable[index].label, label);
            printf("Copied label: %s, %i\n", symbolTable[index].label, symbolTable[index].address);
            index++;
        }
        label = NULL;
        startAddr += 0x02;
        status = readAndParse(file, line, &label, &opcode, &arg1, &arg2, &arg3, &arg4);
    }
    strcpy(symbolTable[index].label, "\0");
    symbolTable[index].address = -1;
}