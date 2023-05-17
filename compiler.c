/*  Sam Voor
    Carter Susi
*/
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAXIDLEN 11   //Identigiers
#define MAXINTLEN 5     //Numbers
#define MAX_SYMBOL_TABLE_SIZE 500


//enums for token type------------------------------------------------------------
typedef enum {
    nulsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym,
    oddsym, eqsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, rparentsym,
    commasym, semicolonsym, periodsym, becomessym, beginsym, endsym, ifsym, thensym,
    whilesym, dosym, callsym, constsym, varsym, procsym, writesym, readsym
} token_type;
//--------------------------------------------------------------------------------

//enums for OP--------------------------------------------------------------------
typedef enum {
    LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SYS

} OPs;
//--------------------------------------------------------------------------------

typedef struct
{
    int kind; // const = 1, var = 2, proc = 3
    char name[10]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted
}symbol;

typedef struct
{
    int Line;
    char OP[10];
    int L;
    int M;
}assembly;
assembly assembly_table[MAX_SYMBOL_TABLE_SIZE];

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

symbol create_symbol(int kind, char *name, int val, int level, int addr, int mark) {
    symbol new_symbol = {kind, "", val, level, addr, mark};
    strncpy(new_symbol.name, name, sizeof(new_symbol.name) - 1);
    return new_symbol;
}
assembly create_assembly(int Line, char *OP, int L, int M) {
    assembly new_assembly = {Line, "", L, M};
    strncpy(new_assembly.OP, OP, sizeof(new_assembly.OP) - 1);
    return new_assembly;
}

static int line_num = 0;
static int L = 0;
static int M = 3;

static int address = 3;
static int mark = 1;
static int level = 0;
static int value = 0;
static int symbol_count = 0;


//structs forassembly-------------------------------------------------
typedef struct instruction {
    int line;
    char* OP; //operation
    int L;  //lexicographical
    int M;  //number
}instruction;
//creating arrays to store instruction structs
instruction instruction_table[MAX_SYMBOL_TABLE_SIZE];
static int it_count = 0;
//--------------------------------------------------------------------------------


//helpers to add instructiond-----------------------------------------------------
instruction create_instruction(int line, char *OP, int L, int M) {
    instruction new_instruction;

    new_instruction.line = line;
    new_instruction.OP = malloc(strlen(OP) + 1);
    strcpy(new_instruction.OP, OP);
    new_instruction.L = L;
    new_instruction.M = M;

    return new_instruction;
}
//--------------------------------------------------------------------------------


//static variables----------------------------------------------------------------
//static int line_num = 0;
static int lvl = 0;
//static int M_addr = 3;

static int operands[] = {4, 5, 6, 7, 9, 10, 11, 12, 13, 14};
//--------------------------------------------------------------------------------


//struct to pass into program, stores the string and token type-------------------
typedef struct {
    char name[50];
    token_type type;
    int levl;
    int sym_addr;
    int cal_val;
} Token;
//--------------------------------------------------------------------------------


//functions-----------------------------------------------------------------------
static int token_index = 0;         //to index tokenList throughout functions
void fix_JMP(int index, int new_m); //sets a new M for JMP
void error(int error_code);         //prints correlating error message
void get();                         //increments index
void block(Token *tokenList);       //starts block of instructions
void program(Token *tokenList);     //starts and ends recursion
void statement(Token *tokenList);
void expression(Token *tokenList);
void condition(Token *tokenList);
void term(Token *tokenList);
void factor(Token *tokenList);

//--------------------------------------------------------------------------------

void fix_JMP(int index, int new_m){
    instruction_table[index].M = new_m;
}
static int new_m_val = 1;
//error func call-----------------------------------------------------------------
//remove ' at index: %d, ' and token_index var before sumbitting, to help debug
void error(int error_code){
    switch (error_code) {
        case 1:
            printf("Error number %d, Use = instead of :=.\n",error_code);
            break;
        case 2:
            printf("Error number %d, = must be followed by a number.\n",error_code);
            break;
        case 3:
            printf("Error number %d, Identifier must be followed by =.\n",error_code);
            break;
        case 4:
            printf("Error number %d, const, var, procedure must be followed by identifier.\n",error_code);
            break;
        case 5:
            printf("Error number %d, Semicolon or comma missing.\n",error_code);
            break;
        case 6:
            printf("Error number %d, Incorrect symbol after procedure declaration.\n",error_code);
            break;
        case 7:
            printf("Error number %d, Statement expected.\n",error_code);
            break;
        case 8:
            printf("Error number %d, Incorrect symbol after statement part in block.\n",error_code);
            break;
        case 9:
            printf("Error number %d, Period expected.\n",error_code);
            break;
        case 10:
            printf("Error number %d, Semicolon between statements missing.\n",error_code);
            break;
        case 11:
            printf("Error number %d, Undeclared identifier.\n",error_code);
            break;
        case 12:
            printf("Error number %d, Assignment to constant or procedure is not allowed.\n",error_code);
            break;
        case 13:
            printf("Error number %d, Assignment operator expected.\n",error_code);
            break;
        case 14:
            printf("Error number %d, call must be followed by an identifier.\n",error_code);
            break;
        case 15:
            printf("Error number %d, Call of a constant or variable is meaningless.\n",error_code);
            break;
        case 16:
            printf("Error number %d, then expected.\n",error_code);
            break;
        case 17:
            printf("Error number %d, Semicolon or end expected.\n",error_code);
            break;
        case 18:
            printf("Error number %d, do expected.\n",error_code);
            break;
        case 19:
            printf("Error number %d, Incorrect symbol following statement.\n",error_code);
            break;
        case 20:
            printf("Error number %d, Relational operator expected.\n",error_code);
            break;
        case 21:
            printf("Error number %d, Expression must not contain a procedure identifier.\n",error_code);
            break;
        case 22:
            printf("Error number %d, Right parenthesis missing.\n",error_code);
            break;
        case 23:
            printf("Error number %d, The preceding factor cannot begin with this symbol.\n",error_code);
            break;
        case 24:
            printf("Error number %d, An expression cannot begin with this symbol.\n",error_code);
            break;
        case 25:
            printf("Error number %d, This number is too large.\n",error_code);
            break;
        case 26:
            printf("Error number %d, Identifier too long.\n",error_code);
            break;
        case 27:
            printf("Error number %d, Invalid symbol.\n",error_code);
            break;
        default:
            printf("Invalid error code %d \n",error_code);
    }
    exit(1);
}

//--------------------------------------------------------------------------------


//get func call-------------------------------------------------------------------
static bool flag = false;//needed to return the first varaible 0
void get(){
    if(flag == true){
        token_index++;
    }
    else{
        flag = true;
    }
}
//--------------------------------------------------------------------------------
int find_token(Token *tokenList, char* token){
    for(int i = 0; i< token_index;i++){
        if(strcmp(token,tokenList[i].name) == 0){
            return i;
        }
    }
    return 0;
}
int check_ident(Token *tokenList, char* token,int place){
for (int i = place-1; i >= 0 ;i--){
        if(tokenList[i].type == procsym){
            return 1;//new proc
        }
        if(strcmp(tokenList[i].name,token) == 0){
            return 2;//used
        }
    }
    return 1;//not used
}
int find_sym(Token * tokenList, int place){
    for (int i = place-1; i >= 0 ;i--){
        if(tokenList[i].type == procsym || tokenList[i].type == varsym || tokenList[i].type == constsym){
            return i;
        }
    }
    return 0;
}

int find_addr(char* token){
    for(int i = 0; i < symbol_count; i++){
        if(strcmp(symbol_table[i].name,token) == 0){
            return i;
        }
    }
    return 0;
}
//block func call-----------------------------------------------------------------
void block(Token *tokenList) {
    address = 3;
    lvl++;
    int space = 3;
    int jmpaddr = it_count;

    instruction_table[it_count] = create_instruction(it_count,"JMP", 0, 0);
    it_count++;

    //constsym------------------------------------------------------------------------
    if (tokenList[token_index].type == constsym){
        tokenList[token_index].levl = lvl;
        tokenList[token_index].sym_addr = it_count;

        do {

            get();

            if (tokenList[token_index].type != identsym){
                error(4);
            }

            int checker = check_ident(tokenList,tokenList[token_index].name,token_index);
            if(checker == 2){
                printf("Error code 28, Identifier name already declared\n");
                exit(1);
            }
            get();

            if (tokenList[token_index].type != eqsym){
                error(3);
            }
            if(tokenList[token_index].type == becomessym){
                error(1);
            }

            get();

            if (tokenList[token_index].type != numbersym){
                error(2);
            }

            get();

        } while(tokenList[token_index].type == commasym);
        if (tokenList[token_index].type != semicolonsym){
            error(5);
        }

        get();
    }
    //--------------------------------------------------------------------------------

    //varsym--------------------------------------------------------------------------
    if (tokenList[token_index].type == varsym) {
        tokenList[token_index].levl = lvl;
        tokenList[token_index].sym_addr = it_count;

        do {
            get();

            if (tokenList[token_index].type != identsym){
                error(4);
            }
            int checker =check_ident(tokenList,tokenList[token_index].name,token_index);
            if(checker == 2){
                printf("Identifier name already declared\n");
                exit(1);
            }

            tokenList[token_index].levl = lvl;

            symbol_table[symbol_count] = create_symbol(2, tokenList[token_index].name, value, lvl, address, mark);
            symbol_count++;
            address++;

            get();

            space++;
        } while(tokenList[token_index].type == commasym);

        if (tokenList[token_index].type != semicolonsym){
                error(5);
        }

        get();
    }
    //--------------------------------------------------------------------------------

    //procsym-------------------------------------------------------------------------
    while (tokenList[token_index].type == procsym) {
        tokenList[token_index].levl = lvl;
        tokenList[token_index].sym_addr = it_count;

        get();
        if (tokenList[token_index].type != identsym){
                error(6);
        }
        int checker =check_ident(tokenList,tokenList[token_index].name,token_index);
            if(checker == 2){
                printf("Identifier name already declared\n");
                exit(1);
            }
        tokenList[token_index].cal_val = new_m_val;
        new_m_val++;

        symbol_table[symbol_count] = create_symbol(3, tokenList[token_index].name, value, lvl, address, mark);
        symbol_count++;
        address++;

        get();

        if (tokenList[token_index].type != semicolonsym){
                error(5);
        }
        get();
        block(tokenList);

        if (tokenList[token_index].type != semicolonsym){
                error(10);
        }

        get();
    }
    fix_JMP(jmpaddr,it_count*3);

    instruction_table[it_count] = create_instruction(it_count,"INC",0,space);
    it_count++;

    if(tokenList[token_index].type != identsym && tokenList[token_index].type != callsym && tokenList[token_index].type != beginsym && tokenList[token_index].type != ifsym && tokenList[token_index].type != whilesym && tokenList[token_index].type != writesym ){
        error(7);
    }
    statement(tokenList);

    lvl--;

    if(lvl == 0){
        instruction_table[it_count] = create_instruction(it_count,"SOU",0,1);
        it_count++;
    }
    else{
       instruction_table[it_count] = create_instruction(it_count,"RTN",0,0);
    it_count++;
    }

    //--------------------------------------------------------------------------------
}
//--------------------------------------------------------------------------------


//program func call---------------------------------------------------------------
void program(Token *tokenList) {
    get();

    block(tokenList);

    if (tokenList[token_index].type != periodsym){
            error(9);
    }

    instruction_table[it_count] = create_instruction(it_count,"EOP",0,3);
    it_count++;
}
//statement func call--------------------------------------------------------------------------------
void statement(Token *tokenList) {
    if (tokenList[token_index].type == identsym) {

        int temp = find_token(tokenList, tokenList[token_index].name);
        int sym = find_sym(tokenList,temp);
        if(tokenList[sym].type == constsym || tokenList[sym].type == procsym){
            error(12);
        }
        if(tokenList[sym].type != varsym){
            error(11);
        }


        int ident_addr = find_addr(tokenList[token_index].name);

        get();

        if (tokenList[token_index].type == becomessym) {
            get();

            expression(tokenList);

            instruction_table[it_count] = create_instruction(it_count,"STO",lvl-symbol_table[ident_addr].level,symbol_table[ident_addr].addr);
            it_count++;
        } else {
            error(13);
        }
    }
    else if (tokenList[token_index].type == callsym) {
        get();

        if (tokenList[token_index].type == identsym) {
            int temp = find_token(tokenList,tokenList[token_index].name);
            int temp2 = tokenList[temp].cal_val;
            int sym = find_sym(tokenList,temp);

            if(tokenList[sym].type == procsym){
                int proc_lvl = find_addr(tokenList[token_index].name);
                instruction_table[it_count] = create_instruction(it_count,"CAL",lvl-symbol_table[proc_lvl].level,temp2);
                it_count++;
            }
            else{
                error(15);
            }

            get();

        } else {
            error(14);
        }
    }
    else if (tokenList[token_index].type == beginsym) {
        get();
        //catch to get out of empty (i)begin (i+1) end statements
        //didnt know to create my own error or add a catch
        if(tokenList[token_index].type == endsym){
             if(tokenList[token_index+1].type == semicolonsym || tokenList[token_index+1].type == periodsym){
                get();
                get();
             }
        }
        statement(tokenList);

        while (tokenList[token_index].type == semicolonsym){

            get();

            if(tokenList[token_index].type == endsym){
                break;
            }

            statement(tokenList);
        }

        if (tokenList[token_index].type != endsym) {
                error(17);
        }

        get();
    }
    else if (tokenList[token_index].type == ifsym) {
        get();

        condition(tokenList);

        int temp_jpc = it_count;

        instruction_table[it_count] = create_instruction(it_count, "JPC", 0, 0);
        it_count++;

        if (tokenList[token_index].type == thensym) {
            get();

            statement(tokenList);

            instruction_table[temp_jpc].M = it_count*3;
        } else {
            error(16);
        }
    }
    else if (tokenList[token_index].type == whilesym) {
        int temp_jmp = it_count;

        get();

        condition(tokenList);

        int temp_jpc = it_count;

        instruction_table[it_count] = create_instruction(it_count, "JPC", 0, 0);
        it_count++;

        if (tokenList[token_index].type == dosym) {
            get();

            statement(tokenList);

            instruction_table[it_count] = create_instruction(it_count, "JMP", 0, temp_jmp);
            it_count++;
            instruction_table[temp_jpc].M = it_count;
        } else {
            error(18);
        }
    }
    else if (tokenList[token_index].type == writesym) {
        get();

        expression(tokenList);
    }
    else {
        error(19);
    }
}
//expression func call--------------------------------------------------------------------------------
void expression(Token *tokenList) {
    if (tokenList[token_index].type == plussym || tokenList[token_index].type == minussym) {
        for(int i = 0; i<10;i++){
                if(tokenList[token_index].type == operands[i]){
                    instruction_table[it_count] = create_instruction(it_count,"OPR",0,i+1);
                    it_count++;
                }
            }

        get();

    }

    term(tokenList);

    while (tokenList[token_index].type == plussym || tokenList[token_index].type == minussym) {
        for(int i = 0; i<10;i++){
                if(tokenList[token_index].type == operands[i]){
                    instruction_table[it_count] = create_instruction(it_count,"OPR",0,i+1);
                    it_count++;
                }
            }

        get();

        term(tokenList);
    }
}
//condition func call--------------------------------------------------------------------------------
void condition(Token *tokenList) {

    if (tokenList[token_index].type == oddsym) {

        get();

        expression(tokenList);
    }
    else {
        expression(tokenList);

        if (tokenList[token_index].type <= eqsym && tokenList[token_index].type >= geqsym) {
            error(20);
        }
        else {
            for(int i = 0; i<10;i++){
                if(tokenList[token_index].type == operands[i]){
                    instruction_table[it_count] = create_instruction(it_count,"OPR",0,i+1);
                    it_count++;
                }
            }

            get();

            expression(tokenList);
        }
    }
}
//term func call--------------------------------------------------------------------------------
void term(Token *tokenList) {
    factor(tokenList);
    while (tokenList[token_index].type == multsym || tokenList[token_index].type == slashsym) {
        for(int i = 0; i<10;i++){
                if(tokenList[token_index].type == operands[i]){
                    instruction_table[it_count] = create_instruction(it_count,"OPR",0,i+1);
                    it_count++;
                }
            }

        get();

        factor(tokenList);
    }
}
//factor func call--------------------------------------------------------------------------------
void factor(Token *tokenList) {

    if (tokenList[token_index].type == identsym) {
        int temp = find_token(tokenList, tokenList[token_index].name);
        int temp_sym = find_sym(tokenList, temp);

        if(tokenList[temp_sym].type == procsym || temp == 0){
            error(21);
        }

        if(tokenList[temp_sym].type == varsym){
            int ident_addr = find_addr(tokenList[token_index].name);
            instruction_table[it_count] = create_instruction(it_count,"LOD",lvl-symbol_table[ident_addr].level,symbol_table[ident_addr].addr);
            it_count++;
        }

        get();

    } else if (tokenList[token_index].type == numbersym) {
        instruction_table[it_count] = create_instruction(it_count,"LIT",0,atoi(tokenList[token_index].name));
        it_count++;

        get();

    } else if (tokenList[token_index].type == lparentsym) {
        get();

        expression(tokenList);

        if (tokenList[token_index].type >= eqsym && tokenList[token_index].type <= geqsym) {
            error(23);
        }
        if (tokenList[token_index].type != rparentsym) {
            error(22);
        }

        get();

    } else {
        error(24);
    }
}

void add_token(char *buf, int length, char **tokens, int *token_count) {
    tokens[*token_count] = malloc(length + 1);
    strncpy(tokens[*token_count], buf, length);
    tokens[*token_count][length] = '\0';
    (*token_count)++;
}

void cleanArray(char**tokens, int *token_count) {
    for (int i = 0; i < *token_count; ++i) {
        bool onlyWhitespace = true;

        for (int j = 0; j < strlen(tokens[i]); ++j) {
            if (!isspace(tokens[i][j])) {
                onlyWhitespace = false;
                break;
            }
        }

        if (onlyWhitespace) {
            free(tokens[i]); // Free the memory of the removed token
            for (int j = i; j < *token_count - 1; ++j) {
                tokens[j] = tokens[j + 1];
            }
            (*token_count)--;
            i--;
        }
    }

    for (int i = 0; i < *token_count; ++i) {
        int start = 0;
        int end = strlen(tokens[i]) - 1;

        while (start <= end && isspace(tokens[i][start])) {
            start++;
        }

        while (end >= start && isspace(tokens[i][end])) {
            end--;
        }

        for (int j = start, k = 0; j <= end; ++j, ++k) {
            tokens[i][k] = tokens[i][j];
        }
        tokens[i][end - start + 1] = '\0';
    }
}

int return_OP(char* oper){
    if(strcmp(oper, "LIT")==0){
        return 1;
    }
    if(strcmp(oper, "OPR")==0 || strcmp(oper, "RTN")==0){
        return 2;
    }
    if(strcmp(oper, "LOD")==0){
        return 3;
    }
    if(strcmp(oper, "STO")==0){
        return 4;
    }
    if(strcmp(oper, "CAL")==0){
        return 5;
    }
    if(strcmp(oper, "INC")==0){
        return 6;
    }
    if(strcmp(oper, "JMP")==0){
        return 7;
    }
    if(strcmp(oper, "JPC")==0){
        return 8;
    }
    if((strcmp(oper, "SYS")==0) || (strcmp(oper, "SOU")==0)|| (strcmp(oper, "EOP")==0))   {
        return 9;
    }
    return 0;
}

int main(int argc, const char * argv[]) {
    FILE* file;
    file = fopen(argv[1], "r");
    char* source_program = malloc(sizeof(char) * (MAX_SYMBOL_TABLE_SIZE * MAXIDLEN));
    int sp_count = 0;
    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    char c;
    int size = 0;
    while ((c = fgetc(file)) != EOF) {
        source_program[sp_count] = c;
        sp_count++;
        size++;
    }

    fseek(file, 0, SEEK_SET);

    char **tokens = malloc(sizeof(char *) * size);
    int token_count = 0;
    char buf[128];
    int buf_index = 0;
    int inside_comment = 0;

    while ((c = fgetc(file)) != EOF) {
        if (inside_comment) {
            if (c == '*' && (c = fgetc(file)) == '/') {
                inside_comment = 0;
            }
            continue;
        }

        if (c == '/' && (c = fgetc(file)) == '*') {
            inside_comment = 1;
            continue;
        }

        if (isalnum(c) || c == '_') {
            buf[buf_index++] = c;
        } else {
            if (buf_index > 0) {
                add_token(buf, buf_index, tokens, &token_count);
                buf_index = 0;
            }
            if (c == ':') {
            char next_c = fgetc(file);
            if (next_c == '=') {
                buf[buf_index++] = ':';
                buf[buf_index++] = '=';
            } else {
                buf[buf_index++] = c;
                ungetc(next_c, file);
            }
            } else {
            buf[buf_index++] = c;
            }
            if (!isspace(c)) {
                add_token(buf, buf_index, tokens, &token_count);
                buf_index = 0;
            }
        }
    }

    if (buf_index > 0) {
        add_token(buf, buf_index, tokens, &token_count);
        buf_index = 0;
    }

    fclose(file);
    cleanArray(tokens, &token_count);
    //--------------------------------------------------------------------------------
printf("Source Program:\n\n");
    for(int i = 0; i< sp_count;i++){
        printf("%c",source_program[i]);
    }
    printf("\n\n");

/*variables for output------------------------------------------------------------------------------------------*/

    char *token_names[] = {"null", "skip", "ID", "NUM", "+", "-", "*", "/", "odd", "=", "<>", "<", "<=", ">", ">=", "(", ")", ",", ";", ".", ":=", "begin", "end", "if", "then", "while", "do", "call", "const", "var", "procedure", "write", "read", "else"};
    int num_names = sizeof(token_names) / sizeof(token_names[0]);

    //variables for lex list
    int num_in_out= sizeof(token_names) * 2;
    int type_tokens[num_in_out];
    char str_out[num_in_out][50];
    int i_counter = 0;
    int s_counter = 0;

/*find token type-----------------------------------------------------------------------------------------------*/
    int halt = 0; //stop if comment
    for(int y = 0; y < token_count ; y++){

        //checking for comments
        if(strcmp(tokens[y],"/*") == 0){
            halt = 1;
        }
        if(strcmp(tokens[y],"*/") == 0){
            halt = 0;
            continue;
        }

        int type = 0;//output for correlating type name

        //if not a comment, find the token type
        if(halt == 0){
            //checking if token has a predefined name
            for(int x = 0; x < num_names; x++){
                if(strcmp(tokens[y], token_names[x]) == 0){
                    type = x;
                }
            }

            //if the token does not have a predefined type, find it
            if(type == 0){
                int len = strlen(tokens[y]);
                if(isalpha(tokens[y][0])){
                    if(len > MAXIDLEN){
                        error(26);
                        continue;
                    }
                    type = 2;
                }
                else if(isdigit(tokens[y][0])){
                    if(len > MAXINTLEN){
                        error(25);
                        continue;
                    }
                    char *endptr;
                    long value = strtol(tokens[y], &endptr, 10);
                    if (*endptr != '\0') {
                        continue;
                    }
                    type = 3;
                }
                else{
                    error(27);
                }
            }

            //proper spacing in output
            if(strlen(tokens[y]) > 8){
                type_tokens[++i_counter] = type;
                if(type == 2){
                    strcpy(str_out[++s_counter], tokens[y]);

                }
            }
            else{
                type_tokens[++i_counter] = type;
                if(type == 2 || type == 3){
                    strcpy(str_out[++s_counter], tokens[y]);
                }
            }
        }
    }

    //creating token struct array-----------------------------------------------------
    Token tokenList[token_count];

    for (int i = 0; i < token_count; i++) {
        strcpy(tokenList[i].name, tokens[i]);
        tokenList[i].type = type_tokens[i+1];
        tokenList[i].levl = 0;
    }
    //--------------------------------------------------------------------------------

    //starting recursive descent parser-----------------------------------------------
    program(tokenList);

    printf("No errors, program is syntactically correct\n\n");


    for(int i = 0; i < it_count;i++){
        if(strcmp(instruction_table[i].OP, "OPR") == 0){
            instruction temp_opr = instruction_table[i];
            instruction temp_LL = instruction_table[i+1];
            instruction_table[i] = temp_LL;
            instruction_table[i + 1] =  temp_opr;
            instruction_table[i].line = temp_opr.line;
            instruction_table[i+1].line = temp_LL.line;
            i++;
        }
        if(strcmp(instruction_table[i].OP, "CAL") == 0){
            int temp_index = instruction_table[i].M;
            instruction_table[i].M = instruction_table[temp_index].M;
        }
    }

    //print to file 'elf.txt'
    FILE *out_file;
    out_file = fopen("elf.txt", "w");
    //printf("\n\nLine\tOP\tL\tM\n");
    for (int i = 0; i < it_count; i++) {
        //printf("%d\t%s\t%d\t%d\n",i, instruction_table[i].OP,instruction_table[i].L,instruction_table[i].M);
        fprintf(out_file,"%d %d %d\n", return_OP(instruction_table[i].OP),instruction_table[i].L,instruction_table[i].M);

    }
    /*printf("\nSymbol Table:\n\nKind | Name        | Value | Level | Address | Mark\n---------------------------------------------------\n");
    for(int u = 0; u < symbol_count; u++){
        printf("  %d  | %11s | %-5d | %-5d | %-7d | %-4d\n",symbol_table[u].kind,symbol_table[u].name,symbol_table[u].val,symbol_table[u].level-1,symbol_table[u].addr,symbol_table[u].mark);
    }*/
    fclose(out_file);
    //--------------------------------------------------------------------------------
    return 0;

}
