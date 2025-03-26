#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_SIZE 200

typedef struct {
    char lexeme[MAX_TOKEN_SIZE];
    int row;
    int col;
} Token;

/* 
   We define a small set of C keywords for demonstration. 
   If we find one of these, we will print its literal text 
   rather than "id". (If you prefer to treat keywords as 'id', 
   just remove them from this list.)
*/
const char *keywords[] = {
    "int", "char", "float", "double", "void",
    "if", "else", "switch", "case", "for",
    "while", "do", "return", "continue", "break",
    "goto", "struct", "union", "enum", "sizeof",
    "typedef", "static", "extern", "const",
    "volatile", "register", "inline", "main"
};
int numKeywords = sizeof(keywords) / sizeof(keywords[0]);

/* Check if a string is one of our recognized C keywords. */
int isKeyword(const char *str) {
    for (int i = 0; i < numKeywords; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Skip an entire line if it starts with '#', treating it as a preprocessor directive. */
void skipPreprocessorDirective(FILE *fp, int *row, int *col) {
    int c;
    // Read until end-of-line or EOF
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        (*col)++;
    }
    if (c == '\n') {
        (*row)++;
        *col = 1;
    }
}

/* Skip comments: single-line (//) or multi-line  */
int skipComment(FILE *fp, int *row, int *col, int firstChar) {
    if (firstChar == '/') {
        int next = fgetc(fp);
        (*col)++;
        if (next == '/') {
            // Single-line comment
            int c;
            while ((c = fgetc(fp)) != EOF && c != '\n') {
                (*col)++;
            }
            if (c == '\n') {
                (*row)++;
                *col = 1;
            }
            return 1; // comment skipped
        } else if (next == '*') {
            // Multi-line comment
            int c;
            while ((c = fgetc(fp)) != EOF) {
                (*col)++;
                if (c == '*') {
                    int c2 = fgetc(fp);
                    (*col)++;
                    if (c2 == '/') {
                        break; // end of multi-line
                    } else {
                        fseek(fp, -1, SEEK_CUR);
                        (*col)--;
                    }
                }
                if (c == '\n') {
                    (*row)++;
                    *col = 1;
                }
            }
            return 1; // comment skipped
        } else {
            // Not a comment, revert the lookahead
            fseek(fp, -1, SEEK_CUR);
            (*col)--;
            return 0; 
        }
    }
    return 0;
}

/* Skip a string literal (anything between double quotes), ignoring tokens inside it. */
int skipStringLiteral(FILE *fp, int *row, int *col, int firstChar) {
    if (firstChar == '\"') {
        int c;
        while ((c = fgetc(fp)) != EOF) {
            (*col)++;
            if (c == '\"') {
                // End of string literal
                break;
            }
            if (c == '\n') {
                // If you want to allow multi-line strings, handle row/col carefully
                (*row)++;
                *col = 1;
            }
        }
        return 1; // string was skipped
    }
    return 0;
}

/* 
   Read the next token from fp, ignoring whitespace, preprocessor directives, 
   comments, and string literals. Returns a Token with lexeme, row, and col. 
   If EOF is reached, lexeme will be "EOF".
*/
Token getNextToken(FILE *fp, int *row, int *col) {
    Token token;
    token.lexeme[0] = '\0';
    int c;

    while ((c = fgetc(fp)) != EOF) {
        (*col)++;

        /* Skip whitespace */
        if (c == ' ' || c == '\t') {
            continue;
        }
        if (c == '\n') {
            (*row)++;
            *col = 1;
            continue;
        }

        /* Preprocessor directive check: if '#' is first non-whitespace on a line */
        if (c == '#') {
            skipPreprocessorDirective(fp, row, col);
            continue;
        }

        /* Comments */
        if (c == '/') {
            // If it's a comment, skip it
            if (skipComment(fp, row, col, c)) {
                continue;
            } else {
                // Otherwise it's just '/', return it as a token
                sprintf(token.lexeme, "%c", c);
                token.row = *row;
                token.col = *col;
                return token;
            }
        }

        /* String literal? */
        if (c == '\"') {
            // Skip the entire string
            skipStringLiteral(fp, row, col, c);
            continue;
        }

        /* 
           If it's alphabetic or underscore, 
           accumulate an identifier/keyword.
        */
        if (isalpha(c) || c == '_') {
            int k = 0;
            char buf[MAX_TOKEN_SIZE];
            buf[k++] = (char)c;

            // Keep reading letters, digits, underscores
            while ((c = fgetc(fp)) != EOF && (isalnum(c) || c == '_')) {
                buf[k++] = (char)c;
                (*col)++;
            }
            buf[k] = '\0';
            // The last read character is not part of the identifier, so push back
            if (c != EOF) {
                fseek(fp, -1, SEEK_CUR);
                (*col)--;
            }

            // Fill the token
            strcpy(token.lexeme, buf);
            token.row = *row;
            token.col = *col;
            return token;
        }

        /* 
           If it's a digit, accumulate a numeric literal 
           (for simplicity, treat sequences of digits as one token).
        */
        if (isdigit(c)) {
            int k = 0;
            char buf[MAX_TOKEN_SIZE];
            buf[k++] = (char)c;

            while ((c = fgetc(fp)) != EOF && (isdigit(c) || c == '.')) {
                buf[k++] = (char)c;
                (*col)++;
            }
            buf[k] = '\0';
            if (c != EOF) {
                fseek(fp, -1, SEEK_CUR);
                (*col)--;
            }

            strcpy(token.lexeme, buf);
            token.row = *row;
            token.col = *col;
            return token;
        }

        /*
           Otherwise, it's likely a single-character token 
           like +, -, =, (, ), {, }, ;, etc. 
           Return it immediately.
        */
        sprintf(token.lexeme, "%c", (char)c);
        token.row = *row;
        token.col = *col;
        return token;
    }

    // If we reach EOF
    strcpy(token.lexeme, "EOF");
    token.row = *row;
    token.col = *col;
    return token;
}

/* 
   Print a token in the desired format:
   - If it's "EOF", we won't print it (or you can choose to print it).
   - If it's an identifier (and not in the keywords list), print "<id,row,col>".
   - If it's a purely numeric string, print "<num,row,col>".
   - Otherwise, print "<token,row,col>".
*/
void printToken(const Token *token) {
    // Don't print EOF if you want to match the sample output exactly
    if (strcmp(token->lexeme, "EOF") == 0) {
        return;
    }

    // Check if lexeme is purely digits => numeric literal
    int isNumeric = 1;
    for (int i = 0; token->lexeme[i] != '\0'; i++) {
        if (!isdigit(token->lexeme[i]) && token->lexeme[i] != '.') {
            isNumeric = 0;
            break;
        }
    }

    // If it's numeric => <num,row,col>
    if (isNumeric) {
        printf("<num,%d,%d>", token->row, token->col);
        return;
    }

    // If it is alphabetical or underscore at start => possibly an identifier or keyword
    if (isalpha(token->lexeme[0]) || token->lexeme[0] == '_') {
        // If it's a recognized keyword => treat as normal token
        if (isKeyword(token->lexeme)) {
            printf("<%s,%d,%d>", token->lexeme, token->row, token->col);
        } else {
            // It's an identifier
            printf("<id,%d,%d>", token->row, token->col);
        }
        return;
    }

    // Else, it's just a single- or multi-character operator/symbol => print literal
    printf("<%s,%d,%d>", token->lexeme, token->row, token->col);
}

int main() {
    FILE *fp = fopen("q1_inp.c", "r");
    if (!fp) {
        printf("Error: cannot open src.c\n");
        return 1;
    }

    int row = 1, col = 1;
    Token token;
    int prevRow = 1;  // We’ll use this to decide if we print a newline for each row.

    while (1) {
        token = getNextToken(fp, &row, &col);
        if (strcmp(token.lexeme, "EOF") == 0) {
            break; // Stop at EOF
        }
        
        // If the token is on a new row, print a newline before printing the token
        if (token.row != prevRow) {
            printf("\n");
            prevRow = token.row;
        }

        printToken(&token);
    }

    fclose(fp);
    return 0;
}
