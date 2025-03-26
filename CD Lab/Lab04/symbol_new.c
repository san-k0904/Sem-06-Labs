#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_SIZE 200
#define MAX_SYMBOLS 100    // Maximum number of symbols in our simple table

// ----------------------------------------------------------------------
// Symbol Table Structures
// ----------------------------------------------------------------------

typedef struct {
    char lexeme[50];   // The name of the symbol (identifier, function name, struct name)
    char type[50];     // The symbol's type (e.g., "int", "struct", "Func", etc.)
    int size;          // Size in bytes (0 if unknown or for functions)
    int index;         // The insertion order index (1-based)
} Symbol;

Symbol SymbolTable[MAX_SYMBOLS];  // Global array for symbol table
int symbolCount = 0;             // How many symbols have been inserted so far

// Display the entire symbol table in the order of insertion.
void displaySymbolTable() {
    printf("\nSymbol Table:\n");
    printf("Index  Lex_Name            Type        Size\n");
    printf("-----  ------------------- ----------- ----\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%-5d  %-20s %-11s %d\n",
               SymbolTable[i].index,
               SymbolTable[i].lexeme,
               SymbolTable[i].type,
               SymbolTable[i].size);
    }
}

// Search the symbol table by lexeme. Returns the array index if found, or -1 if not found.
int searchSymbol(const char *lexeme) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(SymbolTable[i].lexeme, lexeme) == 0) {
            return i;
        }
    }
    return -1;
}

// Insert a new symbol into the table if it doesn't already exist.
void insertSymbol(const char *lexeme, const char *type, int size) {
    int idx = searchSymbol(lexeme);
    if (idx != -1) {
        // Symbol already in the table; do nothing or update if you wish.
        return;
    }

    if (symbolCount < MAX_SYMBOLS) {
        strcpy(SymbolTable[symbolCount].lexeme, lexeme);
        strcpy(SymbolTable[symbolCount].type, type);
        SymbolTable[symbolCount].size = size;
        SymbolTable[symbolCount].index = symbolCount + 1;  // 1-based index
        symbolCount++;
    }
}

// A helper that guesses size for common C types.
int guessSize(const char *type) {
    if (strcmp(type, "int") == 0)    return 4;
    if (strcmp(type, "char") == 0)   return 1;
    if (strcmp(type, "bool") == 0)   return 1;
    if (strcmp(type, "double") == 0) return 8;
    return 0; // default if not recognized
}

// ----------------------------------------------------------------------
// Lexical Analyzer Section
// ----------------------------------------------------------------------

typedef struct {
    char lexeme[MAX_TOKEN_SIZE];
    int row;
    int col;
} Token;

// We define a small set of recognized keywords, including "struct".
const char *keywords[] = {
    "int", "char", "float", "double", "void", "bool",
    "struct", "if", "else", "for", "while", "return", "main"
};
int numKeywords = sizeof(keywords) / sizeof(keywords[0]);

// Check if a string is one of our recognized keywords.
int isKeyword(const char *str) {
    for (int i = 0; i < numKeywords; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Reads from 'fp' and returns the next token (lexeme, row, col), skipping whitespace/newlines.
Token getNextToken(FILE *fp, int *row, int *col) {
    Token token;
    token.lexeme[0] = '\0';
    int c;

    while ((c = fgetc(fp)) != EOF) {
        (*col)++;
        
        // Skip spaces and tabs
        if (c == ' ' || c == '\t') {
            continue;
        }
        // Handle newlines
        if (c == '\n') {
            (*row)++;
            *col = 1;
            continue;
        }

        // If alpha or underscore, accumulate an identifier
        if (isalpha(c) || c == '_') {
            int index = 0;
            char buf[MAX_TOKEN_SIZE];
            buf[index++] = (char)c;

            while ((c = fgetc(fp)) != EOF && (isalnum(c) || c == '_')) {
                buf[index++] = (char)c;
                (*col)++;
            }
            buf[index] = '\0';

            if (c != EOF) {
                fseek(fp, -1, SEEK_CUR);
                (*col)--;
            }

            strcpy(token.lexeme, buf);
            token.row = *row;
            token.col = *col;
            return token;
        }

        // If digit, accumulate a numeric literal
        if (isdigit(c)) {
            int index = 0;
            char buf[MAX_TOKEN_SIZE];
            buf[index++] = (char)c;
            while ((c = fgetc(fp)) != EOF && (isdigit(c) || c == '.')) {
                buf[index++] = (char)c;
                (*col)++;
            }
            buf[index] = '\0';
            if (c != EOF) {
                fseek(fp, -1, SEEK_CUR);
                (*col)--;
            }

            strcpy(token.lexeme, buf);
            token.row = *row;
            token.col = *col;
            return token;
        }

        // Otherwise, treat as single-character token
        token.lexeme[0] = (char)c;
        token.lexeme[1] = '\0';
        token.row = *row;
        token.col = *col;
        return token;
    }

    // If EOF reached
    strcpy(token.lexeme, "EOF");
    token.row = *row;
    token.col = *col;
    return token;
}

// ----------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------
int main() {
    // Open a C file for analysis
    FILE *fp = fopen("q1_inp.c", "r");
    if (!fp) {
        printf("Error: could not open file.\n");
        return 1;
    }

    int row = 1, col = 1;
    Token token, nextToken;
    long filePos;
    int savedRow, savedCol;

    // We'll keep track of the last recognized type for variables/functions.
    static char lastType[50] = "int"; // Default if not recognized

    printf("Lexemes in the order they appear:\n");
    while (1) {
        token = getNextToken(fp, &row, &col);
        if (strcmp(token.lexeme, "EOF") == 0) {
            break;
        }

        // Print each lexeme in the order they appear
        printf("%s ", token.lexeme);

        // If the token is "struct", the next token is the structure name
        if (strcmp(token.lexeme, "struct") == 0) {
            nextToken = getNextToken(fp, &row, &col);
            if (strcmp(nextToken.lexeme, "EOF") == 0) break;

            printf("%s ", nextToken.lexeme); // print the structure name too
            // Insert the structure name into the table with type "struct" and size=0
            insertSymbol(nextToken.lexeme, "struct", 0);
            continue;
        }

        // If the token is a recognized type (int, char, bool, double, float, void),
        // store it in lastType
        if (isKeyword(token.lexeme)) {
            if (strcmp(token.lexeme, "int") == 0 ||
                strcmp(token.lexeme, "char") == 0 ||
                strcmp(token.lexeme, "bool") == 0 ||
                strcmp(token.lexeme, "double") == 0 ||
                strcmp(token.lexeme, "float") == 0 ||
                strcmp(token.lexeme, "void") == 0)
            {
                strcpy(lastType, token.lexeme);
            }
            continue;
        }

        // If the token is an identifier, check if the next token is "(" => function
        if (isalpha(token.lexeme[0]) || token.lexeme[0] == '_') {
            // Save file position
            filePos = ftell(fp);
            savedRow = row;
            savedCol = col;

            nextToken = getNextToken(fp, &row, &col);
            if (strcmp(nextToken.lexeme, "(") == 0) {
                // It's a function name
                // Insert symbol with type = lastType, size=0
                insertSymbol(token.lexeme, lastType, 0);
            } else {
                // Not a function, restore pointer
                fseek(fp, filePos, SEEK_SET);
                row = savedRow;
                col = savedCol;
                // Insert the identifier with type=lastType, size guessed from type
                int size = 0; 
                size = guessSize(lastType);
                insertSymbol(token.lexeme, lastType, size);
            }
            continue;
        }

        // If it's something else (like a bracket, operator, numeric literal, etc.), do nothing special.
    }

    fclose(fp);

    printf("\n");  // End line after printing all lexemes
    displaySymbolTable();  // Finally, display the symbol table

    return 0;
}
