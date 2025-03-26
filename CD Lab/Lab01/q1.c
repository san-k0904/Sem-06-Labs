#include<stdio.h>
#include<stdlib.h>

int main(){
    FILE* in_file= fopen("input1.c","r");
    if(in_file==NULL){
        printf("Error reading input-file\n");
        return EXIT_FAILURE;
    }

    char c;
    int lines=1, character=0;
    while(c!=EOF){
        c=getc(in_file);
        if(c=='\n'){
            lines++;
        }
        else if(c==EOF){
            continue;
        }
        else{
            character++;
        }
    }
    printf("No. of characters: %d and No. of lines: %d\n",character,lines);
    fclose(in_file);
    return 0;

}