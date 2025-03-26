#include<stdio.h>
#include<stdlib.h>

int curr=0;
char str[100];

void valid(){
    printf("\n---------SUCCESS---------\n");
    exit(EXIT_SUCCESS);
}
void invalid(){
    printf("\n---------INVALID---------\n");
    exit(EXIT_FAILURE);
}

void U();
void V();
void W();

void S(){
    U();
    V();
    W();
}
void U(){
    if(str[curr]=='('){
        curr++;
        S();
        if(str[curr]==')'){
            curr++;
            return;
        }
        else{
            printf("%c\n",str[curr]);
            invalid();
        }
    }
    else if(str[curr]=='a'){
        curr++;
        S();
        if(str[curr]=='b'){
            curr++;
            return;
        }
        else{
            printf("%c\n",str[curr]);
            invalid();
        }
    }
    else if( str[curr]=='d'){
        curr++;
        return;
    }
    else{
        printf("%c\n",str[curr]);
        invalid();
    }
}
void V(){
    if(str[curr]=='a'){
        curr++;
        V();
    }
}
void W(){
    if(str[curr]=='c'){
        curr++;
        W();
    }
}

int main(){
    printf("Enter string: ");
    fgets(str,sizeof(str),stdin);
    S();
    if(str[curr]=='$'){
        valid();
    }
    else{
        printf("%c\n",str[curr]);
        invalid();
    }
}