#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>

int curr=0;
char input[100];
void valid(){
    printf("\nSUCCESS\n");
    exit(EXIT_SUCCESS);
}
void invalid(){
    printf("\nINVALID\n");
    exit(EXIT_FAILURE);
}
void T();
void TPrime();
void S(){
    if(input[curr]=='a'){
        curr++;
        return;
    }
    else if( input[curr]=='>'){
        curr++;
        return;
    }
    else if (input[curr]=='('){
        curr++;
        T();
        if(input[curr]==')'){
            curr++;
            return;
        }
        else{
            invalid();
        }
    }
    else{
        invalid();
    }
}
void T(){
    S();
    TPrime();
}
void TPrime(){
    if(input[curr]==','){
        curr++;
        S();
        TPrime();
    }
}
int main(){
    printf("Enter string :");
    fgets(input,sizeof(input),stdin);
    S();
    if(input[curr]=='$'){
        valid();
    }
    else{
        printf("%c\n",input[curr]);
        invalid();
    }
}

