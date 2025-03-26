#include<stdio.h>
#include<stdlib.h>

int curr=0;
char str[100];

void valid(){
    printf("\n---------SUCCESS---------\n");
    exit(EXIT_SUCCESS);
}
void invalid(){
    printf("%c\n",str[curr]);
    printf("\n---------INVALID---------\n");
    exit(EXIT_FAILURE);
}

void A();
void APrime();
void B();

void S(){
    if(str[curr]=='a'){
        curr++;
        A();
        if(str[curr]=='c'){
            curr++;
            B();
            if(str[curr]=='e'){
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
    else{
        invalid();
    }
}
void A(){
    if(str[curr]=='b'){
        curr++;
        APrime();
    }
    else{
        invalid();
    }
}
void APrime(){
    if(str[curr]=='b'){
        curr++;
        APrime();
    }
}
void B(){
    if(str[curr]=='d'){
        curr++;
        return;
    }
    else{
        invalid();
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
        invalid();
    }
}