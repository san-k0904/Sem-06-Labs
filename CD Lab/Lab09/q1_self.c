#include<stdio.h>
#include<stdlib.h>
#include<string.h>
char* terminals[6]={"id","+","*","(",")","$"};
char* productions[7]={"E'->E","E->E+T","E->T","T->T*F","T->F","F->(E)","F->id"};
int prod_head[]={0,1,1,2,2,3,3};
int prod_len[]={1,3,1,3,1,3,1};

char input[100];
char a[3];
int i=0;

char* ParseTable[12][10] = {
    {"s5", "", "", "s4", "", "", "-1", "1", "2", "3"},
    {"", "s6", "", "", "", "accept", "-1", "-1", "-1", "-1"},
    {"", "r2", "s7", "", "r2", "r2", "-1", "-1", "-1", "-1"},
    {"", "r4", "r4", "", "r4", "r4", "-1", "-1", "-1", "-1"},
    {"s5", "", "", "s4", "", "", "-1", "8", "2", "3"},
    {"", "r6", "r6", "", "r6", "r6", "-1", "-1", "-1", "-1"},
    {"s5", "", "", "s4", "", "", "-1", "-1", "9", "3"},
    {"s5", "", "", "s4", "", "", "-1", "-1", "-1", "10"},
    {"", "s6", "", "", "s11", "", "-1", "-1", "-1", "-1"},
    {"", "r1", "s7", "", "r1", "r1", "-1", "-1", "-1", "-1"},
    {"", "r3", "r3", "", "r3", "r3", "-1", "-1", "-1", "-1"},
    {"", "r5", "r5", "", "r5", "r5", "-1", "-1", "-1", "-1"}
};

void printExpected(int state){
    print("EXPECTED symbol: ");
    int first=1;
    for(int j=0;j<6;j++){//traverse throught the ACTION part
        if(ParseTable[state][j][0]=='s'||ParseTable[state][j][0]=='r'||strcmp(ParseTable[state][j][0],"accept")==0){
            if(!first){//means one symbol is already printed
                printf(" or ");
            }
            printf("%s",terminals[j]);//the state which has s or r or accept that terminal value should be there
            first=0;
        }
    }
    printf("\n");
}
int getNextSymbol(){
    a[0]=input[i++];
    //this is done for all terminal values >1
    if(a[0]=='i'&& input[i]=='d'){
        a[1]=input[i++];
        a[2]='\0';
    }
    else{
        a[1]='\0';
    }
    for(int j=0;j<6;j++){//go through the terminals to check if this is present
        if(strcmp(a,terminals[j])==0){
            return j;//this we'll use to check later if the terminal column in parse table shifts or reduce or accepts
        }
    }
    printf("Incorrect symbol %s\n",a);
    exit(EXIT_FAILURE);
}
int main(){
    int stack[50];
    int stackptr=-1;
    printf("Enter the string to take input: ");
    fgets(input,sizeof(input),stdin);
    stack[++stackptr]=0;
    int col=getNextSymbol();
    while(1){
        int curr_state= stack[stackptr];
        if(ParseTable[curr_state][col][0]=='s'){
            stack[++stackptr]= ParseTable[curr_state][col][1]-'0';
            printf("%s (%s)\n","shift",ParseTable[curr_state][col]);
            col=getNextSymbol();
        }
        else if(ParseTable[curr_state][col][0]=='r'){
            int prodn= ParseTable[col][1]-'0';
            stackptr=stackptr-prod_len[prodn];
            int new_reduced_state=stack[stackptr];
            stack[++stackptr] = atoi(ParseTable[new_reduced_state][prod_head[prodn] + 6]);
            printf("%s (%s) %s\n","reduce", ParseTable[curr_state][col], productions[prodn]);
        }
        else if(strcmp(ParseTable[curr_state][col],"accept")==0){
            printf("accept\n");
            break;
        }
        else {
            printf("Error at symbol: %s\n", terminals[col]);
            print_expected(curr_state);
            exit(-1);
        }
    }
    return 0;
}