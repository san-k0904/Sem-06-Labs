#include <stdio.h>      // Include standard I/O functions
#include <stdlib.h>     // Include standard library functions (exit, etc.)
#include <ctype.h>      // Include character type functions (isspace, isdigit, etc.)
#include <string.h>     // Include string functions (strcpy, etc.)

// Define an enumeration for our token types.
typedef enum {
    TOKEN_NUMBER,     // Token type for numeric literals (e.g., 123)
    TOKEN_PLUS,       // Token type for the '+' operator
    TOKEN_MINUS,      // Token type for the '-' operator
    TOKEN_MULTIPLY,   // Token type for the '*' operator
    TOKEN_DIVIDE,     // Token type for the '/' operator
    TOKEN_LPAREN,     // Token type for the '(' character
    TOKEN_RPAREN,     // Token type for the ')' character
    TOKEN_EOF         // Token type to indicate end-of-input
} TokenType;

// Define a structure to hold a token.
typedef struct {
    TokenType type;             // The type of token (from the enumeration above)
    char lexeme[100];           // The string representation of the token (if needed)
} Token;

// Global pointer to the input string (our arithmetic expression)
const char *input;  // This will point to the current character in the input

// Function prototype for getNextToken(), which returns the next token from input.
Token getNextToken();

//---------------------------------------------------------------------------
// Main function: tests the lexical analyzer by processing a sample expression.
//---------------------------------------------------------------------------
int main() {
    // Set the input string to a sample arithmetic expression.
    input = "12 + 34 * (56 - 78) / 90";
    
    // Declare a Token variable to hold each token.
    Token token;
    
    // Loop indefinitely until we reach the end-of-input token.
    while (1) {
        token = getNextToken();      // Get the next token from the input
        
        // If the token type is TOKEN_EOF, break out of the loop.
        if (token.type == TOKEN_EOF)
            break;
        
        // Use a switch statement to print the token based on its type.
        switch (token.type) {
            case TOKEN_NUMBER:   // If the token is a number
                printf("<NUMBER, %s>\n", token.lexeme); // Print as <NUMBER, lexeme>
                break;
            case TOKEN_PLUS:     // If the token is '+'
                printf("<PLUS, %s>\n", token.lexeme);   // Print as <PLUS, +>
                break;
            case TOKEN_MINUS:    // If the token is '-'
                printf("<MINUS, %s>\n", token.lexeme);  // Print as <MINUS, ->
                break;
            case TOKEN_MULTIPLY: // If the token is '*'
                printf("<MULTIPLY, %s>\n", token.lexeme); // Print as <MULTIPLY, *>
                break;
            case TOKEN_DIVIDE:   // If the token is '/'
                printf("<DIVIDE, %s>\n", token.lexeme);   // Print as <DIVIDE, />
                break;
            case TOKEN_LPAREN:   // If the token is '('
                printf("<LPAREN, %s>\n", token.lexeme);   // Print as <LPAREN, (>
                break;
            case TOKEN_RPAREN:   // If the token is ')'
                printf("<RPAREN, %s>\n", token.lexeme);   // Print as <RPAREN, )>
                break;
            default:             // Default case (should not occur)
                break;
        }
    }
    
    return 0; // Exit the program successfully
}

//---------------------------------------------------------------------------
// getNextToken() - Reads the next token from the input string.
//---------------------------------------------------------------------------

Token getNextToken() {
    Token token;                 // Declare a token variable to return later
    token.lexeme[0] = '\0';      // Initialize the lexeme to an empty string
    
    // Skip any whitespace characters (spaces, tabs, etc.)
    while (*input != '\0' && isspace(*input)) {
        input++;               // Advance the input pointer past the whitespace
    }
    
    // If we've reached the end of the input string, return an EOF token.
    if (*input == '\0') {
        token.type = TOKEN_EOF;      // Set token type to TOKEN_EOF
        strcpy(token.lexeme, "EOF"); // Copy "EOF" into the lexeme field
        return token;                // Return the EOF token
    }
    
    // If the current character is a digit, then it starts a numeric literal.
    if (isdigit(*input)) {
        token.type = TOKEN_NUMBER;   // Set token type to TOKEN_NUMBER
        int i = 0;                   // Initialize an index for the lexeme buffer
        
        // Loop to copy all consecutive digits into token.lexeme
        while (*input != '\0' && isdigit(*input)) {
            token.lexeme[i++] = *input; // Copy the current digit into the lexeme
            input++;                  // Advance the input pointer
        }
        token.lexeme[i] = '\0';      // Null-terminate the lexeme string
        return token;                // Return the numeric token
    }
    
    // Check for a '+' operator.
    if (*input == '+') {
        token.type = TOKEN_PLUS;     // Set token type to TOKEN_PLUS
        token.lexeme[0] = *input;      // Store the '+' character in the lexeme
        token.lexeme[1] = '\0';        // Null-terminate the lexeme string
        input++;                     // Advance the input pointer by one
        return token;                // Return the plus token
    }
    
    // Check for a '-' operator.
    if (*input == '-') {
        token.type = TOKEN_MINUS;    // Set token type to TOKEN_MINUS
        token.lexeme[0] = *input;      // Store the '-' character in the lexeme
        token.lexeme[1] = '\0';        // Null-terminate the lexeme string
        input++;                     // Advance the input pointer
        return token;                // Return the minus token
    }
    
    // Check for a '*' operator.
    if (*input == '*') {
        token.type = TOKEN_MULTIPLY; // Set token type to TOKEN_MULTIPLY
        token.lexeme[0] = *input;      // Store the '*' character in the lexeme
        token.lexeme[1] = '\0';        // Null-terminate the lexeme string
        input++;                     // Advance the input pointer
        return token;                // Return the multiplication token
    }
    
    // Check for a '/' operator.
    if (*input == '/') {
        token.type = TOKEN_DIVIDE;   // Set token type to TOKEN_DIVIDE
        token.lexeme[0] = *input;      // Store the '/' character in the lexeme
        token.lexeme[1] = '\0';        // Null-terminate the lexeme string
        input++;                     // Advance the input pointer
        return token;                // Return the division token
    }
    
    // Check for a left parenthesis '('.
    if (*input == '(') {
        token.type = TOKEN_LPAREN;   // Set token type to TOKEN_LPAREN
        token.lexeme[0] = *input;      // Store '(' in the lexeme
        token.lexeme[1] = '\0';        // Null-terminate the lexeme string
        input++;                     // Advance the input pointer
        return token;                // Return the left parenthesis token
    }
    
    // Check for a right parenthesis ')'.
    if (*input == ')') {
        token.type = TOKEN_RPAREN;   // Set token type to TOKEN_RPAREN
        token.lexeme[0] = *input;      // Store ')' in the lexeme
        token.lexeme[1] = '\0';        // Null-terminate the lexeme string
        input++;                     // Advance the input pointer
        return token;                // Return the right parenthesis token
    }
    
    // If none of the above cases match, then we have an unknown character.
    // For simplicity, we skip it and recursively call getNextToken() to get a valid token.
    input++;                       // Skip the unknown character
    return getNextToken();         // Recursively call getNextToken() to process the next character
}
