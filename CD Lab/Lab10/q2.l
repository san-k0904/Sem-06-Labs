%{
    #include "q2.tab.h"
%}
%%
[0-9]+ {return NUMBER;}
"if" {return IF;}
"else" {return ELSE;}
"(" {return LPAREN;}
")" {return RPAREN;}
";" {return SEMI_COL;}
[a-zA-Z_][a-zA-Z_0-9]* {return ID;}
"{"[^}]*"}"             { /* Ignore entire block including braces */ }
[ \t\n]+ {}
. {}
%%
int yywrap(){
    return 1;
}