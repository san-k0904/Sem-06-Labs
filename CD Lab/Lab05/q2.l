%{
    #include<stdio.h>
    #include<stdlib.h>
    int chars=0;
    int blanks=0;
    int words=0;
    int lines=0;    
%}
%%
[ \t] { blanks++; }// it has white space and \t
\n {lines++;}
[a-zA-Z]+ {chars++; words++;}
. {chars++;}
%%
int yywrap(){
    return 1;
}
int main(){
    printf("Enter string: ");
    yylex();
    printf("\nNumber of characters: %d\n",chars);
    printf("Number of blanks: %d\n",blanks);
    printf("Number of lines: %d\n",lines);
    printf("Number of words: %d\n",words);
    return 0;
}