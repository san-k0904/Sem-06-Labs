#include <stdio.h>      // Standard Input/Output library for file operations and printf
#include <stdlib.h>     // Standard Library for general functions like exit
#include <string.h>     // Library for string manipulation functions (strcmp, strcpy, etc.)
#include <ctype.h>      // Library for character handling functions (isalpha, isdigit, etc.)

#define MAX_KEYWORDS 28 // Define the total number of keywords in our keywords array

// Define a structure to represent a token extracted from the input file.
// Each token has a lexeme (the actual text), a type (classification), and a position (row and column).
typedef struct { 
    char lexeme[200];   // Array to store the actual text of the token (up to 199 characters + null terminator)
    char type[50];      // Array to store the token type (for example, "Keyword", "Identifier", etc.)
    int row;            // The row number in the source file where the token was found
    int col;            // The column number in the source file where the token was found
} Token;

// Array of pointers to strings containing the C keywords used in our lexical analysis.
char *keywords[MAX_KEYWORDS] = {
    "int", "char", "float", "double", "void", 
    "if", "else", "switch", "case", "for", 
    "while", "do", "return", "continue", "break", 
    "goto", "struct", "union", "enum", "sizeof", 
    "typedef", "static", "extern", "const", 
    "volatile", "register", "inline", "main"
};

// Function: isKeyword
// Purpose: Check if the given string 'str' is one of the defined C keywords.
// Returns 1 (true) if it is a keyword, 0 (false) otherwise.
int isKeyword(char *str) {
    // Loop over each keyword in the 'keywords' array.
    for (int i = 0; i < MAX_KEYWORDS; i++) {
        // Compare the current keyword with the input string 'str'
        if (strcmp(keywords[i], str) == 0)
            return 1; // Return 1 if a match is found (it is a keyword)
    }
    return 0; // Return 0 if no keyword matches 'str'
}

// Function: getNextToken
// Purpose: Read from the file 'fp' and return the next token along with updating row and column counters.
// Parameters:
//   - fp: Pointer to the input file
//   - row: Pointer to the current row number (will be updated)
//   - col: Pointer to the current column number (will be updated)
Token getNextToken(FILE *fp, int *row, int *col) {
    char c;                    // Variable to hold the current character read from the file
    Token token;               // Create a Token instance to store information about the current token
    token.lexeme[0] = '\0';    // Initialize the lexeme as an empty string

    // Read characters from the file until end-of-file (EOF) is reached
    while ((c = fgetc(fp)) != EOF) {
        (*col)++;            // Increment the column counter for every character read

        // If the character is a space or tab, skip it and continue reading the next character
        if (c == ' ' || c == '\t')
            continue;
        // If the character is a newline, increment the row counter, reset column to 1, and continue
        if (c == '\n') { 
            (*row)++; 
            *col = 1; 
            continue; 
        }

        // ----- Handle Comments -----
        // Check if the character is '/' which may indicate the start of a comment.
        if (c == '/') {
            char next = fgetc(fp); // Read the next character to decide comment type
            if (next == '/') { // If the next character is '/', it's a single-line comment.
                // Read and discard characters until a newline or EOF is reached.
                while ((c = fgetc(fp)) != '\n' && c != EOF)
                    ;
                // After finishing the comment, update row and reset column.
                (*row)++; 
                *col = 1;
                continue; // Continue with the next character after the comment.
            } else if (next == '*') { // If the next character is '*', it's a multi-line comment.
                // Read characters until the end of the multi-line comment is found.
                while ((c = fgetc(fp)) != EOF) {
                    // If a '*' is found and the following character is '/', then the comment ends.
                    if (c == '*' && (next = fgetc(fp)) == '/')
                        break;
                    // If a newline is encountered inside the comment, update row and reset column.
                    if (c == '\n') { 
                        (*row)++; 
                        *col = 1; 
                    }
                }
                continue; // Continue after the multi-line comment.
            } else {
                // If the character following '/' is not part of a comment, move one character back.
                fseek(fp, -1, SEEK_CUR);
            }
        }

        // ----- Arithmetic Operators -----
        // Check if the current character is one of the arithmetic operators: +, -, /, %, *
        if (strchr("+-/%*", c)) {
            sprintf(token.lexeme, "%c", c); // Convert the operator character to a string and store in token.lexeme
            strcpy(token.type, "Arithmetic Operator"); // Set the token type as "Arithmetic Operator"
            token.row = *row;  // Record the current row in the token
            token.col = *col;  // Record the current column in the token
            return token;      // Return the token as the next token
        }

        // ----- Logical Operators -----
        // Check if the current character could be part of a logical operator: &, |, or !
        if (c == '&' || c == '|' || c == '!') {
            char next = fgetc(fp); // Read the next character to check for a two-character operator
            // Check for operators: &&, ||, or !=
            if ((c == '&' && next == '&') || (c == '|' && next == '|') || (c == '!' && next == '=')) {
                sprintf(token.lexeme, "%c%c", c, next); // Combine both characters into token.lexeme
                strcpy(token.type, "Logical Operator");  // Set token type as "Logical Operator"
                (*col)++;       // Increment column counter for the additional character
            } else {
                // If not a two-character operator, move the file pointer back one character
                fseek(fp, -1, SEEK_CUR);
                sprintf(token.lexeme, "%c", c); // Store the single logical operator character
                strcpy(token.type, "Logical Operator"); // Set token type as "Logical Operator"
            }
            token.row = *row;  // Set token row to current row
            token.col = *col;  // Set token column to current column
            return token;      // Return the logical operator token
        }

        // ----- Relational and Assignment Operators -----
        // Check if the current character is one of '=', '>', or '<'
        if (strchr("=><", c)) {
            char next = fgetc(fp); // Read the next character to determine if it's a two-character operator (e.g., '==')
            if (next == '=') {     // If next character is '=', then form a two-character operator
                sprintf(token.lexeme, "%c%c", c, next); // Store both characters in token.lexeme
                strcpy(token.type, "Relational Operator"); // Set token type as "Relational Operator"
                (*col)++;         // Increment the column counter for the extra character
            } else {
                // If not a two-character operator, move one character back
                fseek(fp, -1, SEEK_CUR);
                sprintf(token.lexeme, "%c", c); // Store the single operator character in token.lexeme
                strcpy(token.type, "Relational Operator"); // Set token type as "Relational Operator"
            }
            token.row = *row;  // Record the row position
            token.col = *col;  // Record the column position
            return token;      // Return the relational operator token
        }

        // ----- Special Symbols -----
        // Check if the character is one of the special symbols: {}[]()^*@?,.;:
        if (strchr("{}[]()^*@?,.;:", c)) {
            sprintf(token.lexeme, "%c", c);    // Store the special symbol in token.lexeme
            strcpy(token.type, "Special Symbol"); // Set token type as "Special Symbol"
            token.row = *row;    // Record the row position
            token.col = *col;    // Record the column position
            return token;        // Return the special symbol token
        }

        // ----- String Literals -----
        // Check if the current character is a double quote, indicating the start of a string literal
        if (c == '\"') {
            int k = 0;             // Index 'k' for placing characters into token.lexeme
            token.lexeme[k++] = c; // Store the opening double quote in token.lexeme
            // Continue reading characters until another double quote is found or EOF is reached
            while ((c = fgetc(fp)) != '\"' && c != EOF) {
                token.lexeme[k++] = c; // Append each character to token.lexeme
            }
            token.lexeme[k++] = '\"';  // Append the closing double quote
            token.lexeme[k] = '\0';    // Null-terminate the string literal
            strcpy(token.type, "String Literal"); // Set the token type to "String Literal"
            token.row = *row;      // Record the current row
            token.col = *col;      // Record the current column
            return token;          // Return the string literal token
        }

        // ----- Keywords and Identifiers -----
        // If the character is alphabetic or an underscore, it could be a keyword or identifier.
        if (isalpha(c) || c == '_') { 
            int k = 0;           // Index 'k' for the temporary buffer 'buf'
            char buf[200];       // Temporary buffer to store the potential keyword/identifier
            buf[k++] = c;        // Add the first character to 'buf'
            // Continue reading characters as long as they are alphanumeric or underscore
            while (isalnum((c = fgetc(fp))) || c == '_') {
                buf[k++] = c;    // Append the character to the buffer
                (*col)++;        // Increment column counter for each character read
            }
            buf[k] = '\0';       // Null-terminate the buffer string
            fseek(fp, -1, SEEK_CUR); // Step back one character since the last read character is not part of the identifier

            strcpy(token.lexeme, buf); // Copy the accumulated identifier from 'buf' into token.lexeme
            token.row = *row;    // Record the current row
            token.col = *col;    // Record the current column

            // Check if the accumulated string is a keyword using the isKeyword function
            if (isKeyword(buf))
                strcpy(token.type, "Keyword");    // If it is a keyword, set the token type accordingly
            else
                strcpy(token.type, "Identifier"); // Otherwise, classify it as an identifier

            return token;        // Return the token (keyword or identifier)
        }

        // ----- Numerical Constants -----
        // Check if the current character is a digit, indicating the start of a number.
        if (isdigit(c)) {
            int k = 0;           // Index 'k' for the temporary buffer 'buf'
            char buf[200];       // Temporary buffer to accumulate the number literal
            buf[k++] = c;        // Store the first digit in the buffer
            // Continue reading digits (and possible decimal point) to build the complete number
            while (isdigit((c = fgetc(fp))) || c == '.') {
                buf[k++] = c;    // Append each digit or '.' to the buffer
                (*col)++;        // Increment the column counter
            }
            buf[k] = '\0';       // Null-terminate the number string
            fseek(fp, -1, SEEK_CUR); // Move the file pointer one character back since the last character read is not part of the number

            strcpy(token.lexeme, buf); // Copy the complete number from 'buf' into token.lexeme
            strcpy(token.type, "Numerical Constant"); // Set the token type as "Numerical Constant"
            token.row = *row;    // Record the current row
            token.col = *col;    // Record the current column
            return token;        // Return the numerical constant token
        }
    }

    // If EOF is reached, set the token to indicate End Of File.
    strcpy(token.lexeme, "EOF");         // Set the lexeme to "EOF"
    strcpy(token.type, "End of File");     // Set the token type to "End of File"
    return token;                          // Return the EOF token
}

int main() {
    // Open the input file "q1_inp.c" for reading.
    FILE *fp = fopen("q1_inp.c", "r");

    // Check if the file was successfully opened; if not, print an error and exit.
    if (!fp) {
        printf("Error: Unable to open file.\n");
        return 1;  // Exit with an error code
    }

    Token token;         // Declare a variable to store tokens received from getNextToken
    int row = 1, col = 1; // Initialize row and column counters (starting at row 1, column 1)

    // Loop to process the entire file token by token.
    while (1) {
        token = getNextToken(fp, &row, &col); // Retrieve the next token from the file
        // If the token lexeme equals "EOF", break out of the loop as we've reached the end of the file.
        if (strcmp(token.lexeme, "EOF") == 0)
            break;
        // Print the token's details: its lexeme, type, row, and column.
        printf("Lexeme: %-15s Type: %-20s Row: %d, Col: %d\n",
               token.lexeme, token.type, token.row, token.col);
    }

    fclose(fp); // Close the input file to free resources.
    return 0;   // Return 0 to indicate successful execution.
}
