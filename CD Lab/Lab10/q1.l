%{
    #include "q1.tab.h"
%}
%%
[0-9]+ {return NUMBER;}
"'"."'" {return CHAR_CONST;}
"=" {return EQUAL;}
";" {return SEMI_COL;}
"int" {return INT;}
"char" {return CHAR;}
"float" {return FLOAT;}
"," {return COMMA;}
[a-zA-Z_][a-zA-Z0-9_]* { return IDENTIFIER; }
[ \t\n] {}
. {}
%%
int yywrap(){
    return 1;
}