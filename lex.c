/*  Sam Voor
    Carter Susi
    COP3402 Fall 2022
    Programming Assignment 2
*/
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAXIDLEN 11   //Identigiers can be a maximum of 11 characters in length.
#define MAXINTLEN 5     //Numbers can be a maximum of 5 digits in length.


int main(int argc, const char * argv[]) {

/*get size of file----------------------------------------------------------------------------------------------*/
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        fprintf(stderr,"Error opening file %s\n", fp);
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);
/*--------------------------------------------------------------------------------------------------------------*/

    printf("Source Program:\n");

/*get tokens----------------------------------------------------------------------------------------------------*/
    // iterate variables
    int in_count = 0;
    int tok_count = 0;
    int toks_count = 0;

    //allocating arrays
    char *input = (char *)malloc((file_size + 1) * sizeof(char));
    char *token = (char *)malloc((file_size + 1) * sizeof(char));
    char tokens[file_size+1][50];

    while (fgets(input, file_size, fp) != NULL) {
        printf("%s",input);
        in_count = 0;
        //each line
        while (input[in_count] != '\0') {
            //if special char
            if (ispunct(input[in_count])) {
                if(input[in_count] == ':' && input[in_count+1] == '='){
                    char temp[3] = {input[in_count],input[in_count+1], '\0'};
                    strcpy(tokens[++toks_count], temp);
                    in_count++;
                }
                else if(input[in_count] == '/' && input[in_count+1] == '*'){
                    char temp[3] = {input[in_count],input[in_count+1], '\0'};
                    strcpy(tokens[++toks_count], temp);
                    in_count++;
                }
                else if(input[in_count] == '*' && input[in_count+1] == '/'){
                    char temp[3] = {input[in_count],input[in_count+1], '\0'};
                    strcpy(tokens[++toks_count], temp);
                    in_count++;
                }
                else if(input[in_count] == '<' && input[in_count+1] == '>'){
                    char temp[3] = {input[in_count],input[in_count+1], '\0'};
                    strcpy(tokens[++toks_count], temp);
                    in_count++;
                }
                else if(input[in_count] == '<' && input[in_count+1] == '='){
                    char temp[3] = {input[in_count],input[in_count+1], '\0'};
                    strcpy(tokens[++toks_count], temp);
                    in_count++;
                }
                else if(input[in_count] == '<' && input[in_count+1] == '='){
                    char temp[3] = {input[in_count],input[in_count+1], '\0'};
                    strcpy(tokens[++toks_count], temp);
                    in_count++;
                }
                else{
                    //storing chars before special char as token
                    if (tok_count > 0) {
                        token[tok_count] = '\0';
                        strcpy(tokens[++toks_count], token);
                        tok_count = 0;
                    }
                    char temp[2] = {input[in_count], '\0'};
                    strcpy(tokens[++toks_count], temp);
                }

            }
            // if space, tab, new line
            else if (isspace(input[in_count])) {
                //storing token
                if (tok_count > 0) {
                    token[tok_count] = '\0';
                    strcpy(tokens[++toks_count], token);
                    tok_count = 0;
                }
            }
            //add char to token[] if conditions not met
            else {
                token[tok_count++] = input[in_count];
            }
            //iterate through input[]
            in_count++;
        }
    }

/*--------------------------------------------------------------------------------------------------------------*/

    printf("\n\nLexeme Table:\n\nlexeme\t\ttoken type\n");
    char *token_names[] = {"null", "skip", "ID", "NUM", "+", "-", "*", "/", "odd", "=", "<>", "<", "<=", ">", ">=", "lparent", "rparent", ",", ";", ".", ":=", "begin", "end", "if", "then", "while", "do", "call", "const", "var", "procedure", "write", "read", "else"};
    int num_names = sizeof(token_names) / sizeof(token_names[0]);

    //variables for lex list
    int num_in_out= sizeof(token_names) * 2;
    int type_out[num_in_out];
    char str_out[num_in_out][50];
    int i_counter = 0;
    int s_counter = 0;

/*--------------------------------------------------------------------------------------------------------------*/

/*find token type-----------------------------------------------------------------------------------------------*/
    int halt = 0; //stop if comment
    for(int y = 1; y < toks_count + 1; y++){

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
                        printf("%s is not a valid identifier, length exceeds 11.\n",tokens[y]);
                        continue;
                    }
                    type = 2;
                }
                else if(isdigit(tokens[y][0])){
                    if(len > MAXINTLEN){
                        printf("%s is not a valid digit, length exceeds 5.\n",tokens[y]);
                        continue;
                    }
                    char *endptr;
                    long value = strtol(tokens[y], &endptr, 10);
                    if (*endptr != '\0') {
                        printf("%s is not a valid identifier or digit, identifiers cannot begin with a digit, digits cannot contain a letter.\n",tokens[y]);
                        continue;
                    }
                    type = 3;
                }
                else{
                    printf("%s is not a valid identifier symbol, valid symbols are+ - * / = <> < <= > >= , ; . :=\n",tokens[y]);
                }
            }

            //proper spacing in output
            if(strlen(tokens[y]) > 8){
                printf("%s\t%d\n",tokens[y], type);
                type_out[++i_counter] = type;
                if(type == 2){
                    strcpy(str_out[++s_counter], tokens[y]);

                }
            }
            else{
                printf("%s\t\t%d\n",tokens[y], type);
                type_out[++i_counter] = type;
                if(type == 2 || type == 3){
                    strcpy(str_out[++s_counter], tokens[y]);
                }
            }
        }
    }

/*--------------------------------------------------------------------------------------------------------------*/
    int q = 0;
    printf("\nLexeme List: \n");
    for(int y = 1; y< i_counter; y++){
        if(type_out[y] == 2 || type_out[y] ==3){
            printf("%d|%s", type_out[y], str_out[++q]);
        }
        else{
            printf("%d", type_out[y]);
        }

        if(y+1 == i_counter){
            ;
        }
        else{
            printf(" | ");
        }
    }

    for(int y = 0; y < toks_count; y++){
         free(tokens[y]);
    }
    free(tokens);
    fclose(fp);
    return 0;
}



