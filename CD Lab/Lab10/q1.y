%{
    #include<stdio.h>
    #include<stdlib.h>
    extern int yylex(void);
    extern int yyerror(char *msg);
%}

%token IDENTIFIER INT CHAR FLOAT EQUAL SEMI_COL COMMA NUMBER CHAR_CONST

%%
declaration_stmt: datatype identifier_list SEMI_COL {printf("Valid Statement\n");exit(EXIT_SUCCESS);}
;
datatype: INT
| CHAR
| FLOAT
;
identifier_list: id
| identifier_list COMMA id
;
id: IDENTIFIER
| IDENTIFIER EQUAL NUMBER
| IDENTIFIER EQUAL CHAR_CONST
;
%%

int yyerror(char *msg){
    printf("INVALID DECALARATION STATEMENT\n");
    exit(EXIT_FAILURE);
}
int main(){
    printf("Enter expression:\n");
    yyparse();
    return 0;
}