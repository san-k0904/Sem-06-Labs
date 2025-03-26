%{
#include<stdio.h>
#include<stdlib.h>
int vcount=0;
int ccount=0;
%}
%%
[aeiouAEIOU] {vcount++;}
[a-zA-Z] {ccount++;}
. {continue;}
%%
int yywrap(){
    return 1;
}
int main(int argc, char* argv[]){
    if(argc<2){
        printf("You should have more than 1 value in terminal\n");
        perror("Less argument");
        exit(EXIT_FAILURE);
    }
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    yyin=fp;
    yylex();
    fclose(fp);

    // Print the results (or any other processing result).
    printf("Vowel count: %d\n", vcount);
    printf("constant count: %d\n", ccount);

    return 0;
}