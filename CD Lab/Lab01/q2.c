#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(){
    FILE* first= fopen("input1.c","r");
    if(first==NULL){
        perror("Error opening input file\n");
        return EXIT_FAILURE;
    }
    FILE* last= fopen("input1.c","r");
    if(last==NULL){
        fclose(first);
        perror("Error opening input file\n");
        return EXIT_FAILURE;
    }
    FILE* out= fopen("output2.c","w+");
    if(out==NULL){
        fclose(first);
        fclose(last);
        perror("Error opening output file\n");
        return EXIT_FAILURE;
    }
    if (fseek(last, 0, SEEK_END) != 0) {//fseek returns non-zero value when it fails
        perror("fseek failed");
        fclose(first);
        fclose(last);
        fclose(out);
        return EXIT_FAILURE;
    }
    long fileSize = ftell(last);
    if (fileSize <= 0) {
        fprintf(stderr, "File is empty or an error occurred.\n");
        fclose(first);
        fclose(last);
        fclose(out);
        return EXIT_FAILURE;
    }

    if (fseek(last, -1, SEEK_END) != 0) {
        perror("fseek failed");
        fclose(first);
        fclose(last);
        fclose(out);
        return EXIT_FAILURE;
    }

    int c= fgetc(last);//fgetc returns an integer and not a character(we can take it as char also as it changes the ascii value back to char)
    while(ftell(last)>0){
        fputc(c,out);
        if(fseek(last,-2,SEEK_CUR)!=0){//basically if error occurs it return non 0 value and so that means we are already at beginning
            break;
        }
        c=fgetc(last);
    }
    fputc(c,out);//WHY THE FUCK IS IT THAT WHEN I COMMENT THIS IT WORKS PROPERLY
    c='\n';
    fputc(c,out);
    // fseek(out,0,SEEK_SET);
    char *str="The file size is: ";
    // fwrite(str,sizeof(char),strlen(str),out);
    // fputc(fileSize,out);//doesn't put integer...putw is only for binary files so use fprintf for inputting in text file
    fprintf(out,"The filesize is: %ld",fileSize);
    rewind(out);
    int outC=fgetc(out);
    while(outC!=EOF){
        printf("%c",(char)outC); //this or putc(outC,stdout);
        outC=fgetc(out);
    }
    printf("\n");
    fclose(first);
    fclose(last);
    fclose(out);
    return 0;
}