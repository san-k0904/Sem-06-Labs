#include <stdio.h>      // For file I/O functions (fopen, fgetc, fseek, ftell, fclose)
#include <stdlib.h>     // For exit() and general utilities
#include <string.h>     // For string functions (strcmp, strcpy)
#include <ctype.h>      // For character type functions (isalpha, isdigit, isalnum)

#define MAX_TOKEN_SIZE 200  // Maximum size for a token's lexeme
#define MAX_LINE_SIZE 1024  // Maximum size for a temporary line buffer (if needed)

// Token structure: holds the token's lexeme (its text) along with the row and column
// where it was found in the input file.
typedef struct {
    char lexeme[MAX_TOKEN_SIZE];  // The string representation of the token
    int row;                      // The row (line) number where the token starts
    int col;                      // The column number where the token starts
} Token;

/*
   getNextToken()

   Reads the next token from the file pointer fp while updating the row and col
   counters. A token is defined as:
     - an identifier: a sequence of letters/digits/underscores starting with a letter or underscore,
     - a numeric literal: a sequence of digits (and possibly a decimal point),
     - or a single-character token (such as '(' or '{', etc.).
   Whitespace and newlines are skipped (while updating row and col).
*/
Token getNextToken(FILE *fp, int *row, int *col) {
    Token token;                    
    token.lexeme[0] = '\0';         // Initialize the lexeme to an empty string
    int c;                          // Variable to hold the current character

    // Loop: read characters until a valid token is formed or EOF is reached.
    while ((c = fgetc(fp)) != EOF) {
        (*col)++;  // Increment column counter for every character read

        // Skip spaces and tabs.
        if (c == ' ' || c == '\t')
            continue;
        
        // On newline, update row and reset col.
        if (c == '\n') {
            (*row)++;
            *col = 1;
            continue;
        }
        
        // If the character is alphabetic or an underscore, start reading an identifier.
        if (isalpha(c) || c == '_') {
            int index = 0;            // Index into our temporary buffer
            char buf[MAX_TOKEN_SIZE]; // Buffer to accumulate the identifier
            buf[index++] = (char)c;   // Save the first character
            // Read subsequent characters while they are alphanumeric or underscore.
            while ((c = fgetc(fp)) != EOF && (isalnum(c) || c == '_')) {
                buf[index++] = (char)c;
                (*col)++;           // Update col for each additional character
            }
            buf[index] = '\0';        // Null-terminate the string

            // The last character read is not part of the token, so move the file pointer back one.
            if (c != EOF) {
                fseek(fp, -1, SEEK_CUR);
                (*col)--;
            }
            strcpy(token.lexeme, buf); // Copy the identifier into the token's lexeme
            token.row = *row;          // Record the row where token started
            token.col = *col;          // Record the column where token ended (or started; here we use current col)
            return token;              // Return the identifier token
        }
        
        // If the character is a digit, accumulate a numeric literal.
        if (isdigit(c)) {
            int index = 0;
            char buf[MAX_TOKEN_SIZE];
            buf[index++] = (char)c;
            // Continue while digits or a dot (for floating-point numbers) are found.
            while ((c = fgetc(fp)) != EOF && (isdigit(c) || c == '.')) {
                buf[index++] = (char)c;
                (*col)++;
            }
            buf[index] = '\0';
            if (c != EOF) {            // Put back the last character read that is not part of the number.
                fseek(fp, -1, SEEK_CUR);
                (*col)--;
            }
            strcpy(token.lexeme, buf); // Copy the numeric literal into the token's lexeme
            token.row = *row;
            token.col = *col;
            return token;              // Return the numeric token
        }
        
        // For any other character, treat it as a single-character token (e.g., '(', '{', etc.)
        token.lexeme[0] = (char)c;      
        token.lexeme[1] = '\0';         // Null-terminate the token string
        token.row = *row;               // Set the token's row
        token.col = *col;               // Set the token's column
        return token;                   // Return the token
    }
    
    // If we reach EOF, set the token's lexeme to "EOF" and return it.
    strcpy(token.lexeme, "EOF");
    token.row = *row;
    token.col = *col;
    return token;
}

/*
   Main Function

   This function opens the input C file and repeatedly calls getNextToken() to
   process the file. It looks for tokens that indicate the start of a structure or
   a function definition:
     - If the token "struct" is encountered, the next token is assumed to be the structure name.
       A token of the form <STRUCT, structureName, row, col> is printed.
     - If an identifier is encountered, we "peek" ahead by using ftell to save the current file offset
       (and the current row and col). We then call getNextToken() to see if the next token is a "(".
       If it is, then the identifier is treated as a function name and a token <FUNCTION, functionName, row, col>
       is printed. If not, we restore the file pointer (and row/col) so that the peeked token is processed normally.
*/
int main() {
    FILE *fp = fopen("q1_inp.c", "r");  // Open the input file (change filename as needed)
    if (!fp) {                          // Check if the file was opened successfully
        printf("Error: cannot open input file\n");
        return 1;
    }
    
    int row = 1, col = 1;   // Initialize row and column counters
    Token token;          // Variable to hold the current token
    Token nextToken;      // Variable to hold a token read for lookahead
    long pos;             // Variable to store the file offset (using ftell)
    int savedRow, savedCol;  // Variables to save the row and column before lookahead

    // Loop until the end of file is reached.
    while (1) {
        token = getNextToken(fp, &row, &col);  // Get the next token from the file
        
        // If the token's lexeme is "EOF", then we have reached the end of the file; exit loop.
        if (strcmp(token.lexeme, "EOF") == 0)
            break;
        
        // Check for a structure definition.
        // If the token equals "struct", then the next token should be the structure name.
        if (strcmp(token.lexeme, "struct") == 0) {
            nextToken = getNextToken(fp, &row, &col);  // Get the next token (structure name)
            // Print the structure token in the format: <STRUCT, structureName, row, col>
            printf("<STRUCT,%s,%d,%d>\n", nextToken.lexeme, nextToken.row, nextToken.col);
            continue;  // Move to the next token in the file.
        }
        
        // Check for a potential function definition:
        // A function is detected when an identifier is immediately followed by "(".
        if (isalpha(token.lexeme[0]) || token.lexeme[0] == '_') {
            // Save current file position using ftell
            pos = ftell(fp);
            // Save current row and column counters before peeking ahead.
            savedRow = row;
            savedCol = col;
            
            // Get the next token to check if it is "(".
            nextToken = getNextToken(fp, &row, &col);
            
            // If the next token is "(", then our previous identifier is a function name.
            if (strcmp(nextToken.lexeme, "(") == 0) {
                // Print the function token in the format: <FUNCTION, functionName, row, col>
                printf("<FUNCTION,%s,%d,%d>\n", token.lexeme, token.row, token.col);
            } else {
                // Otherwise, restore the file pointer and row/col counters so the peeked token isn't lost.
                fseek(fp, pos, SEEK_SET);
                row = savedRow;
                col = savedCol;
            }
            continue;  // Continue with the next token.
        }
        
        // For any other token, we simply ignore it.
    }
    
    fclose(fp);  // Close the input file.
    return 0;    // End the program successfully.
}
