#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#define MAX_LINE 1024

int check_Directive(const char* line){
    const char *valid_directives[] = {
        "define", "include", "if", "ifdef", "ifndef", "else",
        "elif", "endif", "undef", "error", "pragma"
    };
    int num_directives = sizeof(valid_directives) / sizeof(valid_directives[0]);

    while(*line && isspace(*line)){
        line++;
    }
    if(*line!='#'){//means not starting with #
        return 0;
    }
    line++;//go ahead of # i.e. at include or define or ifndef

    /* Skip spaces after '#' */
    while (*line && isspace((unsigned char)*line)) {
        line++;
    }

    for (int i = 0; i < num_directives; i++) {
        size_t len = strlen(valid_directives[i]);
        if (strncmp(line, valid_directives[i], len) == 0 &&
            (line[len] == '\0' || isspace((unsigned char)line[len]) || line[len] == '<' || line[len] == '\"')) {
            return 1; /* Valid directive found */
        }

    }
    return 0; /* Not a valid preprocessor directive */

}

int main(){
    char line[MAX_LINE];
    FILE* fin=fopen("src.c","r");
    FILE* fout=fopen("out2.c","w");
    int index = 0;
    int ch;

    // Read character by character
    while ((ch = fgetc(fin)) != EOF) {
        if (ch != '\n') {
            // Accumulate characters until a newline or end-of-line
            if (index < MAX_LINE - 1) {
                line[index++] = ch;
            }
        }else {
            // End of the line: terminate the string
            line[index] = '\0';

            // If the line is NOT a valid preprocessor directive, write it out
            if (!check_Directive(line)) {
                for (int i = 0; i < index; i++) {
                    fputc(line[i], fout);
                }
                // Write the newline character
                fputc('\n', fout);
            }
            index = 0;  // Reset for the next line
        }
    }
    // If the last line does not end with a newline, process it here
    if (index > 0) {
        line[index] = '\0';
        if (!check_Directive(line)) {
            for (int i = 0; i < index; i++) {
                fputc(line[i], fout);
            }
        }
    }

    fclose(fin);
    fclose(fout);
    return 0;

}