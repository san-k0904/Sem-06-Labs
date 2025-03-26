%{
    #include<stdio.h>
    #include<stdlib.h>
    #include<string.h>
    int yylex(void);
    int yyerror(char *msg);
%}
%token IF ELSE SEMI_COL ID NUMBER LPAREN RPAREN LBRACE RBRACE

%%
stmt: matched_stmt {printf("Valid Statement\n"); exit(EXIT_SUCCESS);}
| unmatched_stmt
;

matched_stmt: IF LPAREN expr RPAREN matched_stmt ELSE matched_stmt 
| other_stmt
| block_stmt
;

unmatched_stmt: IF LPAREN expr RPAREN stmt
| IF LPAREN expr RPAREN matched_stmt ELSE unmatched_stmt
;

expr: ID
| NUMBER
;

other_stmt: SEMI_COL
;

block_stmt: LBRACE RBRACE SEMI_COL
;
%%

int yyerror(char *msg){
    printf("INVALID \n");
    exit(EXIT_FAILURE);
}

int main(){
    printf("Enter expression:\n");
    yyparse();
    return 0;
}