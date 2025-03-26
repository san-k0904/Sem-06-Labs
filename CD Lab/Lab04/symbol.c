#include <stdio.h>      // For standard I/O functions (fopen, fgetc, fseek, ftell, etc.)
#include <stdlib.h>     // For general utilities (malloc, exit)
#include <string.h>     // For string handling functions (strcmp, strcpy)
#include <ctype.h>      // For character testing functions (isalpha, isalnum, isdigit)

#define TableLength 30          // Length of the hash table
#define MAX_TOKEN_SIZE 200      // Maximum size for a token's lexeme

// ----------------------------------------------------------------------
// Symbol Table Section
// ----------------------------------------------------------------------

// Define the structure for a symbol table entry.
// For each symbol, we store its lexeme (name), size (if needed),
// type (for functions, it will be "Func"; for structures, "struct"; for others, the data type),
// and scope (here we use 'G' for global, for simplicity).
struct Symbol {
    char lexeme[20];    // The symbol's name (identifier)
    int size;           // Size of the symbol (could be computed later; 0 for functions)
    char type[20];      // The data type of the symbol; for functions, we store "Func"
    char scope;         // Scope of the symbol (e.g., 'G' for global)
};

// Each entry in the hash table is stored as a node in a linked list.
struct ListElement {
    struct Symbol sym;           // The symbol table entry stored in this node
    struct ListElement *next;    // Pointer to the next node in the chain (for collisions)
};

// TABLE is an array (of size TableLength) of pointers to ListElement.
// Each TABLE[i] is the head of a linked list of symbols hashing to index i.
struct ListElement *TABLE[TableLength];

// Initialize the symbol table by setting each TABLE slot to NULL.
void Initialize(){
    for (int i = 0; i < TableLength; i++){
         TABLE[i] = NULL;
    }
}

// Display the entire symbol table by iterating through each linked list.
void Display(){
    printf("Symbol Table:\n");
    printf("Lexeme\tSize\tType\tScope\n");
    for (int i = 0; i < TableLength; i++){
         struct ListElement *ele = TABLE[i];
         while(ele != NULL){
             printf("%s\t%d\t%s\t%c\n", ele->sym.lexeme, ele->sym.size, ele->sym.type, ele->sym.scope);
             ele = ele->next;
         }
    }
}

// HASH function: Compute a hash value for a given string by summing its ASCII values.
// Then take the modulo with TableLength to determine the index.
int HASH(char *str){
    int sum = 0;
    while(*str){
       sum += *str;
       str++;
    }
    return sum % TableLength;
}

// SEARCH function: Check whether a symbol with a given lexeme already exists
// in the symbol table. Returns 1 if found, else returns 0.
int SEARCH(char *str){
    int index = HASH(str);
    struct ListElement *cur = TABLE[index];
    while(cur != NULL){
         if(strcmp(cur->sym.lexeme, str) == 0)
             return 1;
         cur = cur->next;
    }
    return 0;
}

// INSERT function: Insert a symbol into the symbol table if it does not already exist.
void INSERT(struct Symbol sym){
    if(SEARCH(sym.lexeme) == 1){
         return; // Do not insert duplicate entries.
    }
    int val = HASH(sym.lexeme);                   // Compute hash value for the symbol's lexeme.
    struct ListElement *cur = (struct ListElement *)malloc(sizeof(struct ListElement));
    cur->sym = sym;                               // Store the symbol into the new node.
    cur->next = NULL;
    if(TABLE[val] == NULL){
         TABLE[val] = cur;                        // No collision: place node here.
    } else {
         struct ListElement *ele = TABLE[val];
         while(ele->next != NULL)                  // Traverse to the end of the list.
             ele = ele->next;
         ele->next = cur;                          // Append the new node at the end.
    }
}

// ----------------------------------------------------------------------
// Lexical Analyzer Section (Functions and Structures)
// ----------------------------------------------------------------------

// Define a simple token structure for our lexical analyzer.
typedef struct {
    char lexeme[MAX_TOKEN_SIZE];  // The token text
    int row;                      // Row number where the token is found
    int col;                      // Column number where the token is found
} Token;

// A list of keywords for our analyzer. We include "struct" since we need to detect structure definitions.
const char *keywords[] = {
    "int", "char", "float", "double", "void", "struct",
    "if", "else", "for", "while", "do", "return", "main"
};
int numKeywords = sizeof(keywords) / sizeof(keywords[0]);

// isKeyword: Check if a given string matches one of our known keywords.
int isKeyword(const char *str) {
    for (int i = 0; i < numKeywords; i++){
         if(strcmp(str, keywords[i]) == 0)
             return 1;
    }
    return 0;
}

/*
   getNextToken()

   A very simple lexical analyzer that reads characters from the file fp and
   generates tokens. Tokens include identifiers (which start with a letter or underscore),
   numeric literals, and single-character tokens (such as '(' or '{').
   Whitespace and newlines are skipped (while updating row and col).
*/
Token getNextToken(FILE *fp, int *row, int *col) {
    Token token;                          // Declare a token variable.
    token.lexeme[0] = '\0';               // Initialize the token's lexeme to an empty string.
    int c;                                // Variable to hold the current character.
    
    // Loop until we form a valid token or reach end-of-file.
    while ((c = fgetc(fp)) != EOF) {
        (*col)++;                       // Increment column for every character read.
        
        // Skip spaces and tabs.
        if (c == ' ' || c == '\t')
            continue;
        
        // On encountering a newline, increment row and reset column.
        if (c == '\n') {
            (*row)++;
            *col = 1;
            continue;
        }
        
        // If the character is alphabetic or an underscore, start forming an identifier.
        if (isalpha(c) || c == '_') {
            int index = 0;
            char buf[MAX_TOKEN_SIZE];
            buf[index++] = (char)c;     // Store the first character.
            // Continue reading while characters are alphanumeric or underscore.
            while ((c = fgetc(fp)) != EOF && (isalnum(c) || c == '_')) {
                buf[index++] = (char)c;
                (*col)++;
            }
            buf[index] = '\0';           // Null-terminate the identifier string.
            if (c != EOF) {              // The last character read is not part of the token.
                fseek(fp, -1, SEEK_CUR); // Move the file pointer back by one.
                (*col)--;
            }
            strcpy(token.lexeme, buf);   // Copy the identifier into the token's lexeme.
            token.row = *row;
            token.col = *col;
            return token;                // Return the identifier token.
        }
        
        // If the character is a digit, form a numeric literal.
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
        
        // For any other character, treat it as a single-character token.
        token.lexeme[0] = (char)c;
        token.lexeme[1] = '\0';
        token.row = *row;
        token.col = *col;
        return token;
    }
    
    // If EOF is reached, return a token with lexeme "EOF".
    strcpy(token.lexeme, "EOF");
    token.row = *row;
    token.col = *col;
    return token;
}

/*
   Main function for the lexical analyzer and symbol table insertion.
   
   It processes the input C file (e.g., "q1_inp.c") token by token.
   When the token "struct" is encountered, the next token is assumed to be the structure name.
   When an identifier is encountered, we "peek" ahead using ftell/fseek. If the next token is "(",
   then the identifier is treated as a function name.
   For functions, we set the symbol table entry type to "Func".
   For structures, we set the type to "struct".
   Each symbol is inserted with a global scope ('G') and a size of 0 (for simplicity).
*/
int main() {
    FILE *fp = fopen("q1_inp.c", "r");   // Open the input file.
    if (!fp) {
        printf("Error: cannot open input file\n");
        return 1;
    }
    
    Initialize();   // Initialize the symbol table.
    
    int row = 1, col = 1;  // Initialize row and column counters.
    Token token, nextToken;  // Variables to store the current token and for lookahead.
    long pos;                // Variable to store file position using ftell.
    int savedRow, savedCol;  // Variables to save row and column counters for lookahead.
    
    // Process tokens until EOF is reached.
    while (1) {
        token = getNextToken(fp, &row, &col);  // Get the next token.
        if (strcmp(token.lexeme, "EOF") == 0)
            break;  // Exit loop if end-of-file.
        
        // Check for structure definitions.
        // If the token is "struct", then the next token should be the structure name.
        if (strcmp(token.lexeme, "struct") == 0) {
            nextToken = getNextToken(fp, &row, &col);  // Get the structure name.
            // Create a symbol table entry for the structure.
            struct Symbol sym;
            strcpy(sym.lexeme, nextToken.lexeme);  // Set the lexeme to the structure name.
            sym.size = 0;                          // Size is set to 0 (could be computed later).
            strcpy(sym.type, "struct");            // For structures, type is "struct".
            sym.scope = 'G';                       // Assume global scope.
            INSERT(sym);                           // Insert into the symbol table.
            printf("<STRUCT,%s,%d,%d>\n", sym.lexeme, nextToken.row, nextToken.col);
            continue;
        }
        
        // Check for function definitions.
        // A function is detected when an identifier is immediately followed by "(".
        if (isalpha(token.lexeme[0]) || token.lexeme[0] == '_') {
            // Save the current file position and row/col counters using ftell.
            pos = ftell(fp);
            savedRow = row;
            savedCol = col;
            nextToken = getNextToken(fp, &row, &col);  // Peek at the next token.
            if (strcmp(nextToken.lexeme, "(") == 0) {
                // We detected a function definition.
                struct Symbol sym;
                strcpy(sym.lexeme, token.lexeme);     // The function name.
                sym.size = 0;                         // Size is not applicable for functions.
                strcpy(sym.type, "Func");             // Mark the type as "Func" for functions.
                sym.scope = 'G';                      // Assume global scope.
                INSERT(sym);                          // Insert into the symbol table.
                printf("<FUNCTION,%s,%d,%d>\n", token.lexeme, token.row, token.col);
            } else {
                // Not a function; restore the file pointer and row/col counters.
                fseek(fp, pos, SEEK_SET);
                row = savedRow;
                col = savedCol;
            }
            continue;
        }
        // For our purposes, we ignore other tokens.
    }
    
    // After processing, display the complete symbol table.
    printf("\n--- Symbol Table ---\n");
    Display();
    
    fclose(fp);  // Close the input file.
    return 0;    // End the program.
}
