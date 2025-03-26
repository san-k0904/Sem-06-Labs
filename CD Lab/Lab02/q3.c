#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN 256

/* List of C keywords */
const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto",
    "if", "inline", "int", "long", "register", "restrict", "return",
    "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
    "union", "unsigned", "void", "volatile", "while"
};
int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

/* Check if the token exactly matches a C keyword */
int is_keyword(const char *token) {
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(token, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int main(void) {
    FILE *fin = fopen("input.c", "r");
    if (fin == NULL) {
        fprintf(stderr, "Error opening input file\n");
        return 1;
    }
    FILE *fout = fopen("output.c", "w");
    if (fout == NULL) {
        fprintf(stderr, "Error opening output file\n");
        fclose(fin);
        return 1;
    }

    int ch;
    char token[MAX_TOKEN];
    int token_index = 0;

    /* Read input character by character */
    while ((ch = fgetc(fin)) != EOF) {
        /* If the character can start an identifier (letter or underscore) */
        if (isalpha(ch) || ch == '_') {
            token_index = 0;
            /* Collect the identifier */
            token[token_index++] = (char)ch;
            while ((ch = fgetc(fin)) != EOF && (isalnum(ch) || ch == '_')) {
                if (token_index < MAX_TOKEN - 1) {
                    token[token_index++] = (char)ch;
                }
            }
            token[token_index] = '\0';

            /* If it is a C keyword, convert the token to uppercase */
            if (is_keyword(token)) {
                for (int i = 0; token[i] != '\0'; i++) {
                    token[i] = toupper(token[i]);
                }
            }
            /* Write the token to the output file character by character */
            for (int i = 0; token[i] != '\0'; i++) {
                fputc(token[i], fout);
            }
            /* If we broke out of the identifier loop because of a non-identifier
               character (and not EOF), output that character as well */
            if (ch != EOF) {
                fputc(ch, fout);
            }
        } else {
            /* For any non-identifier character, just output it */
            fputc(ch, fout);
        }
    }

    fclose(fin);
    fclose(fout);
    return 0;
}
