%{
    #include<stdio.h>
    #include<stdlib.h>
    int scan_count=0;
    int print_count=0;
%}
%%
"scanf" {scan_count++; fprintf(yyout,"READ");}
"printf" {print_count++;  fprintf(yyout,"WRITE");}
.|\. {fprintf(yyout, "%s", yytext);}
%%
int yywrap(){
    return 1;
}
int main(int argc, char* argv[]){
    if(argc<=2){
        printf("Less arguments\n");
        perror("LESS");
        exit(EXIT_FAILURE);
    }
    yyin=fopen(argv[1],"r");
    yyout=fopen(argv[2],"w+");
    yylex();
    return 0;
}