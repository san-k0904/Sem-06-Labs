%{
    #include<stdio.h>
    #include<stdlib.h>
    int posInteger=0;
    int negInteger=0;
    int posf=0;
    int negf=0;
%}

digit [0-9]
digits {digit}+
float {digits}"."{digits}
%%
[+]?{digits} {posInteger++;}
[-]{digits} {negInteger--;}
[+]?{float} {posf++;}
[-]{float} {negf++;}
.|\. {continue;}
%%
int yywrap(){
    return 1;
}
int main(){
    printf("Enter all the numbers:\n");
    yylex();
    printf("Total positive integer: %d\n",posInteger);
    printf("Total negative integer: %d\n",negInteger);

    printf("Total positive floating: %d\n",posf);
    printf("Total negative floating: %d\n",negf);
    return 0;
}