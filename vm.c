/* Sam Voor
* Carter Susi
*/
/*
 compile:
            gcc -o vm vm.c
            ./vm input.txt
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define ARRAY_SIZE 500

typedef struct instruction {
    int OP;
    int L;
    int M;
} instruction;

typedef struct PM_0 {
    int BP;
    int SP;
    int PC;
    instruction IR;
} PM_0;

int pas[ARRAY_SIZE];

int base(int BP, int L) {
    int arb = BP;
    while (L > 0) {
        arb = pas[arb];
        L--;
    }
    return arb;
}

struct PM_0 CPU;
struct instruction TEXT;

int main(int argc, const char * argv[]) {

//create intital values--------------------------------------------------------------------------------------------------
    CPU.BP = 499;
    CPU.SP = CPU.BP+1;
    CPU.PC = 0;
    for(int x = 0; x<ARRAY_SIZE;x++){
        pas[x] = 0;
    }
    int TEXT[3];
    int init_SP = CPU.SP;   //to remember the initial SP
    int init_BP = CPU.BP;   //to remember the initial BP
    bool AR = false;        //if activation record is made, set to true
    printf("\t\t\t\tPC\tBP\tSP\tstack\n");
    printf("Initial Values:\t\t%4d%4d%4d\n", CPU.PC, CPU.BP, CPU.SP);
//--------------------------------------------------------------------------------------------------------------------


//gather input--------------------------------------------------------------------------------------------------------
    FILE* input;
    input = fopen(argv[1], "r");

    if(input == NULL){
        fprintf(stderr, "File not found... ending program.\n");
        return 0;
    }
    else{
        int i = 0;//  i = the number of input
        while (!feof(input)){
            fscanf(input,"%d%d%d", &pas[i], &pas[i+1], &pas[i+2]);
            i+=3;
        }
    }
//--------------------------------------------------------------------------------------------------------------------


//creating the TEXT array to represent the current instruction-------------------------------------------------------
    TEXT[0] = pas[0];
    TEXT[1] = pas[1];
    TEXT[2] = pas[2];
//--------------------------------------------------------------------------------------------------------------------


//while loop until EOP------------------------------------------------------------------------------------------------

    do{
        switch(TEXT[0]){
//--------------------------------------------------------------------------------------------------------------------------
//case 1 (LIT),
//--------------------------------------------------------------------------------------------------------------------------
            case(1):
                CPU.SP -= 1;
                pas[CPU.SP] = TEXT[2];
                printf("\tLIT\t%4d%4d%4d%4d%4d", TEXT[1],TEXT[2],CPU.PC,CPU.BP,CPU.SP);
                break;
//--------------------------------------------------------------------------------------------------------------------------
//case 2 (RTN),
//--------------------------------------------------------------------------------------------------------------------------
            case(2):
                switch (TEXT[2]) {
                    case(0):
                        if(TEXT[1] == 0){
                            CPU.SP = CPU.BP + 1;
                            CPU.BP = pas[CPU.SP-2];
                            CPU.PC = pas[CPU.SP-3];
                            AR = false;
                        }
                        break;
                case (1):
                    pas[CPU.SP++] = pas[CPU.SP++] + pas[CPU.SP];
                    CPU.SP++;
                    break;
                // SUB
                case (2):
                    pas[CPU.SP + 1] = pas[CPU.SP + 1] - pas[CPU.SP];
                    CPU.SP++;
                    break;
                // MUL
                case (3):
                    pas[CPU.SP + 1] = pas[CPU.SP + 1] * pas[CPU.SP];
                    CPU.SP++;
                    break;
                // DIV
                case (4):
                    pas[CPU.SP + 1] = pas[CPU.SP + 1] / pas[CPU.SP];
                    CPU.SP++;
                    break;
                // EQL
                case (5):
                    pas[CPU.SP + 1] = (pas[CPU.SP + 1] == pas[CPU.SP]) ? 1 : 0;
                    CPU.SP++;
                    break;
                // NEQ
                case (6):
                    pas[CPU.SP + 1] = (pas[CPU.SP + 1] != pas[CPU.SP]) ? 1 : 0;
                    CPU.SP++;
                    break;
                // LSS
                case (7):
                    pas[CPU.SP + 1] = (pas[CPU.SP + 1] < pas[CPU.SP]) ? 1 : 0;
                    CPU.SP++;
                    break;
                // LEQ
                case (8):
                    pas[CPU.SP + 1] = (pas[CPU.SP + 1] <= pas[CPU.SP]) ? 1 : 0;
                    CPU.SP++;
                    break;
                // GTR
                case (9):
                    pas[CPU.SP + 1] = (pas[CPU.SP + 1] > pas[CPU.SP]) ? 1 : 0;
                    CPU.SP++;
                    break;
                // GEQ
                case (10):
                    pas[CPU.SP + 1] = (pas[CPU.SP + 1] >= pas[CPU.SP]) ? 1 : 0;
                    CPU.SP++;
                    break;
                }
                printf("\tRTN\t%4d%4d%4d%4d%4d", TEXT[1],TEXT[2],CPU.PC,CPU.BP,CPU.SP);
                break;
//--------------------------------------------------------------------------------------------------------------------------
//case 3 (LOD),
//--------------------------------------------------------------------------------------------------------------------------
            case(3):
                CPU.SP--;
                pas[CPU.SP] = pas[base(CPU.BP, TEXT[1]) - TEXT[2]];
                printf("\tLOD\t%4d%4d%4d%4d%4d", TEXT[1],TEXT[2],CPU.PC,CPU.BP,CPU.SP);
                break;
//--------------------------------------------------------------------------------------------------------------------------
//case 4 (STO),
//--------------------------------------------------------------------------------------------------------------------------
            case(4):
                pas[base(CPU.BP, TEXT[1]) - TEXT[2]] = pas[CPU.SP];
                CPU.SP++;
                printf("\tSTO\t%4d%4d%4d%4d%4d", TEXT[1],TEXT[2],CPU.PC,CPU.BP,CPU.SP);
                break;
//--------------------------------------------------------------------------------------------------------------------------
//case 5 (CAL),
//--------------------------------------------------------------------------------------------------------------------------
            case(5):
                AR = true;
                CPU.BP = CPU.SP - 1;
                pas[CPU.SP-1] = init_SP - 1;
                pas[CPU.SP-2] = init_BP;
                pas[CPU.SP-3] = CPU.PC;
                CPU.PC = TEXT[2];
                printf("\tCAL\t%4d%4d%4d%4d%4d", TEXT[1],TEXT[2],CPU.PC,CPU.BP,CPU.SP);
                break;
//--------------------------------------------------------------------------------------------------------------------------
//case 6 (INC), subtracts M number of locals from SP, when stack output is printed it is printed from SP until original SP
//--------------------------------------------------------------------------------------------------------------------------
            case(6):
                CPU.SP = CPU.SP - TEXT[2];
                printf("\tINC\t%4d%4d%4d%4d%4d", TEXT[1],TEXT[2],CPU.PC,CPU.BP,CPU.SP);
                break;
//--------------------------------------------------------------------------------------------------------------------------
//case 7 (JMP), changes TEXT struct to the specified PC Counter (TEXT[2])
//--------------------------------------------------------------------------------------------------------------------------
            case(7):
                CPU.PC = TEXT[2];
                printf("\tJMP\t%4d%4d%4d%4d%4d", TEXT[1],TEXT[2],CPU.PC,CPU.BP,CPU.SP);
                break;
//--------------------------------------------------------------------------------------------------------------------------
//case 8 (JPC),
//--------------------------------------------------------------------------------------------------------------------------
            case(8):
                if (pas[CPU.SP] == 0) {
                  CPU.PC = TEXT[2];
                  CPU.SP++;
                }
                printf("\tJPC\t%4d%4d%4d%4d%4d", TEXT[1],TEXT[2],CPU.PC,CPU.BP,CPU.SP);
                break;
//--------------------------------------------------------------------------------------------------------------------------
//case 9 (SYS),
//--------------------------------------------------------------------------------------------------------------------------
            case(9):
                switch(TEXT[2]){
                    case(1):
                        printf("Output result is: %4d\n",pas[CPU.SP]);
                        CPU.SP += TEXT[2];
                        printf("\tSOU\t%4d%4d%4d%4d%4d", TEXT[1],TEXT[2],CPU.PC,CPU.BP,CPU.SP);
                        break;
                    case(2):
                        CPU.SP--;
                        int user_in;
                        while(1){
                            printf("Please Enter an integer: ");
                            if(scanf("%d", &user_in)==1){
                                pas[CPU.SP] = user_in;
                                break;
                            }else{
                                printf("Invalid input...\n");
                                scanf("%*s");
                            }
                        }
                        printf("\tSIN\t%4d%4d%4d%4d%4d", TEXT[1],TEXT[2],CPU.PC,CPU.BP,CPU.SP);
                        break;
                    case(3):
                        break;
                }
                break;
//--------------------------------------------------------------------------------------------------------------------------
//
        //end of case statements
        }

//--------------------------------------stores and displays next instruction --------------------------------------------

//display the next instruction---------------------------------------------------------------------------------------

//if the SP has changed, display the stack---------------------------------------------------------------------------

        if (CPU.SP != init_SP) {
            for (int y = init_SP - 1; y >= CPU.SP; y--) {
                if (y == CPU.BP && AR == true) {
                    printf("|");
                }
                printf("%4d", pas[y]);
            }
        }

        printf("\n");
//--------------------------------------------------------------------------------------------------------------------
        TEXT[0] = pas[CPU.PC];
        TEXT[1] = pas[CPU.PC+ 1];
        TEXT[2] = pas[CPU.PC + 2];
        CPU.PC+=3;

//--------------------------------------------------------------------------------------------------------------------
    //end of while loop
    }while(!(TEXT[0] == 9 && TEXT[1] == 0 && TEXT[2] == 3));

//print EOP statement-------------------------------------------------------------------------------------------------
    printf("\tEOP\t%4d%4d%4d%4d%4d", TEXT[1],TEXT[2],CPU.PC,CPU.BP,CPU.SP);
    if(CPU.SP != init_SP){
        for(int y = init_SP-1 ; y >= CPU.SP ; y--){
            if(y == CPU.BP && AR == true){
                printf("|");
            }
            printf("%4d", pas[y]);
        }
    }
printf("\n");
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
    return 0;

}
