#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char input[100];
char a[3];
char* symbol_col[6] = {"id", "+", "*", "(", ")", "$"};
int i = 0;
int production_len[7] = {1, 3, 1, 3, 1, 3, 1};
int prod_head[7] = {0, 1, 1, 2, 2, 3, 3}; //represents the NON-TERMINALS
char* prod[7] = {"E'->E", "E->E+T", "E->T", "T->T*F", "T->F", "F->(E)", "F->id"};

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

int getnextsymbol() {
    a[0] = input[i++];
    if (a[0] == 'i' && input[i] == 'd') {
        a[1] = input[i++];
        a[2] = '\0';
    } else {
        a[1] = '\0';
    }

    for (int j = 0; j < 6; j++) {
        if (strcmp(a, symbol_col[j]) == 0) {
            return j;
        }
    }

    printf("Incorrect symbol: %s\n", a);
    exit(-1);
}

void print_expected(int state) {
    printf("ERROR expected: ");
    int first = 1;
    for (int j = 0; j < 6; j++) {
        if (ParseTable[state][j][0] == 's' || ParseTable[state][j][0] == 'r' || strcmp(ParseTable[state][j], "accept") == 0) {
            if (!first) {//If first is not true (i.e., we’ve already printed an expected symbol), 
                //we print " or " to separate this expected symbol from the previous ones.
                printf(" or ");
            }
            printf("%s", symbol_col[j]);
            first = 0;
        }
    }
    printf("\n");
}

int main() {
    int stack[50];
    int stackptr = -1;
    printf("Enter input: ");
    scanf("%s", input);
    stack[++stackptr] = 0;
    int col = getnextsymbol();
    while (1) {
        int s = stack[stackptr];
        if (ParseTable[s][col][0] == 's') {
            stack[++stackptr] = ParseTable[s][col][1] - '0';//shift to the other state and -'0' gives it in numerical form
            printf("%s (%s)\n","shift", ParseTable[s][col]); // Print shift action
            col = getnextsymbol();
        } else if (ParseTable[s][col][0] == 'r') {
            int prodn = ParseTable[s][col][1] - '0';//this holds the production number using which it will reduce
            int prodlen = production_len[prodn];
            for (int k = 0; k < prodlen; k++) {
                stackptr--;//remove(move ptr back) from stack as many as production length
            }
            int t = stack[stackptr];
            stack[++stackptr] = atoi(ParseTable[t][prod_head[prodn] + 6]);//since after reducing it will check if the current 
            //top of stack and prod_head[prodn]=NON-TERMINAL goes to some other GOTO state and in my production GOTO state starts after 6(ACTION) values
            printf("%s (%s) %s\n","reduce", ParseTable[s][col], prod[prodn]); // Print reduce action
        } else if (strcmp(ParseTable[s][col], "accept") == 0) {
            printf("accept\n");
            break;
        } else {
            printf("Error at symbol: %s\n", symbol_col[col]);
            print_expected(s);
            exit(-1);
        }
    }
    return 0;
}