#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

typedef struct{
    char type[50];
    char value[50];
    int row;
    int col;
}Token;
typedef struct{
    const char* input;
    int pos;
    int row;
    int col;
}Lexer;

void Lexer_init(Lexer* lexer,const char* source){
    lexer->input=source;
    lexer->pos=0;
    lexer->row=1;
    lexer->col=1;
}
Token getNextToken(Lexer* lexer){
    Token token={"","",lexer->row,lexer->col};
    while(lexer->pos<strlen(lexer->input)&& isspace(lexer->input[lexer->pos])){
        if(lexer->input[lexer->pos]=='\n'){
            lexer->row++;
            lexer->col=1;
        }
        else{
            lexer->col++;
        }
        lexer->pos++;
    }
    if(lexer->pos>= strlen(lexer->input)){
        strcpy(token.type,"EOF");
        return token;
    }

    char current= lexer->input[lexer->pos];

    //check if keyword or identifier
    if(isalpha(current)){
        char id[50];
        int i=0;
        char* keywords[3]={"main","int","char"};
        while(lexer->pos<strlen(lexer->input)&& isalnum(lexer->input[lexer->pos])){
            id[i++]=lexer->input[lexer->pos++];
            lexer->col++;
        }
        id[i]='\0';
        //first check for keyword
        int key_flag=0;
        for(int j=0;j<3;j++){
            if(strcmp(id,keywords[j])==0){
                strcpy(token.type,id);
                strcpy(token.value,id);
                key_flag=1;
                break;
            }
        }
        if(key_flag==0){
            //now it must be identifier
            strcpy(token.type,"id");
            strcpy(token.value,id);
        }
        token.col-=strlen(id);
        return token;
    }
    //check if it is a number
    if(isdigit(current)){
        char num[50];
        int i=0;
        while(lexer->pos<strlen(lexer->input)&& isdigit(lexer->input[lexer->pos])){
            num[i++]=lexer->input[lexer->pos++];
            lexer->col++;
        }
        num[i]='\0';
        strcpy(token.type,"num");
        strcpy(token.value,num);
        token.col-=strlen(num);
        return token;
    }

    //check for (,),{,},;, , ,=
    // we actually want all the single values even if an error to give token...parser handles if there are any error 
    //we can do this and then one more block of code for any remaining or since these too are single single values
    //we can combine them together
    // char special_symbols[7]={'(',')','{','}',';',',','='};
    // char value[2]={current,'\0'};
    // for(int i=0;i<7;i++){
    //     if(current==special_symbols[i]){
    //         strcpy(token.type,value);
    //         strcpy(token.value,value);
    //         return token;
    //     }
    // }

    char value[2]={current,'\0'};
    strcpy(token.type,value);
    strcpy(token.value,value);
    lexer->pos++;
    lexer->col++;
    return token;
}