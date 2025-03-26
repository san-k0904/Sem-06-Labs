#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(){
    FILE* in=fopen("src.c","r");
    FILE* out=fopen("out.c","w");
    int ch= fgetc(in);
    while(ch!=EOF){
        if(ch==' ' || ch=='\t'){
            fputc(' ',out);
            while((ch=fgetc(in))!=EOF &&(ch==' '||ch=='\t'));
        }
        if(ch!=' '){//if instead of else if so that even after spaces this gets checked
            if(ch=='/'){
                if((ch=fgetc(in))!=EOF && ch=='/'){
                    while((ch=fgetc(in))!=EOF && ch!='\n');//go on till you encounter new line
                }
                else if(ch=='*'){
                    while((ch=fgetc(in))!=EOF && ch!='*');
                    if((ch=fgetc(in))!=EOF && ch=='/'){
                        ch=fgetc(in);//go to the next character and put that coz put is outside 
                        continue;
                    }
                }
            }
        }
        if(ch!=EOF || ch!=' '|| ch!='\t'){
            fputc(ch,out);
            ch=fgetc(in);
        }
    }
    fclose(in);
    fclose(out);
    return 0;
}