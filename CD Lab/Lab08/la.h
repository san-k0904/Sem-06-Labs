#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Token structure - Generic, language-independent
typedef struct {
    char type[20];  // Token type (e.g., "id", "num", "int", "if", etc.)
    char value[50]; // Token value (e.g., variable name, number value)
    int row;        // Row number for error reporting
    int col;        // Column number for error reporting
} Token;

// Lexer structure - Generic, language-independent
typedef struct {
    const char* input; // Source code string
    size_t pos;        // Current position in input
    int row;           // Current row for error reporting
    int col;           // Current column for error reporting
} Lexer;

// Lexer function declarations
void lexer_init(Lexer* lexer, const char* source);
Token getNextToken(Lexer* lexer);

void lexer_init(Lexer* lexer, const char* source) {
    lexer->input = source;
    lexer->pos = 0;
    lexer->row = 1;
    lexer->col = 1;
}

/* 
 * Language-specific: Modify this function to change token recognition rules
 * - Add/remove keywords based on the target language
 * - Adjust operator recognition (e.g., add new operators like "and", "or")
 * - Modify identifier/number recognition if syntax differs
 */
Token getNextToken(Lexer* lexer) {
    Token token = {"", "", lexer->row, lexer->col};
    
    // Skip whitespace - Language-independent
    //and increment lexer position and row col value accordingly
    while (lexer->pos < strlen(lexer->input) && isspace(lexer->input[lexer->pos])) {
        if (lexer->input[lexer->pos] == '\n') {
            lexer->row++;
            lexer->col = 1;
        } else {
            lexer->col++;
        }
        lexer->pos++;
    }

    // End of input - Language-independent
    if (lexer->pos >= strlen(lexer->input)) {
        strcpy(token.type, "EOF");
        return token;
    }

    char current = lexer->input[lexer->pos];
    
    // Language-specific: Keyword and identifier recognition
    // For C: recognizes "main", "int", "char", "if", "else" as keywords
    if (isalpha(current)) {
        char id[50] = "";
        int i = 0;
        while (lexer->pos < strlen(lexer->input) && isalnum(lexer->input[lexer->pos])) {
            id[i++] = lexer->input[lexer->pos++];
            lexer->col++;
        }
        id[i] = '\0';
        
        // Modify this section for different languages' keywords
        if (strcmp(id, "main") == 0 || strcmp(id, "int") == 0 || strcmp(id, "char") == 0 ||
            strcmp(id, "if") == 0 || strcmp(id, "else") == 0) {
            strcpy(token.type, id);//give the keyword as it is in token.type coz our grammar wants it that way
            strcpy(token.value, id);
        } else {
            strcpy(token.type, "id");//only if not keyword then it takes it as identifier
            strcpy(token.value, id);
        }
        token.col -= strlen(id);//coz the value actually starts are col-length(id)
        return token;
    }
    
    // Language-specific: Number recognition
    // For C: simple decimal integers
    if (isdigit(current)) {
        char num[50] = "";
        int i = 0;
        while (lexer->pos < strlen(lexer->input) && isdigit(lexer->input[lexer->pos])) {
            num[i++] = lexer->input[lexer->pos++];
            lexer->col++;
        }
        num[i] = '\0';
        strcpy(token.type, "num");
        strcpy(token.value, num);
        token.col -= strlen(num);
        return token;
    }

    // Language-specific: Operator and symbol recognition
    // For C: =, ==, <, <=, >, >=, !=, +, -, *, /, %, (, ), {, }, [, ], ;, ,
    char value[3] = {current, '\0', '\0'};
    lexer->pos++;
    lexer->col++;

    if (current == '=') {
        if (lexer->pos < strlen(lexer->input) && lexer->input[lexer->pos] == '=') {
            value[1] = '=';//because we have already incremented pos after taking current value in it's 0th index i.e. =
            lexer->pos++;
            lexer->col++;
            strcpy(token.type, "relop");//because our grammar wants it to be relop type
            strcpy(token.value, "==");
        } else {
            strcpy(token.type, "=");//since our grammar uses = in assign_stat->id=expn so we take = as it is and since
            //it = doesn't have any specific non-terminal for it
            strcpy(token.value, "=");
        }
        return token;
    }
    
    if (current == '<' || current == '>') {
        if (lexer->pos < strlen(lexer->input) && lexer->input[lexer->pos] == '=') {
            value[1] = '=';
            lexer->pos++;
            lexer->col++;
        }
        strcpy(token.type, "relop");//for <= and >=
        strcpy(token.value, value);
        return token;
    }
    
    if (current == '!' && lexer->pos < strlen(lexer->input) && lexer->input[lexer->pos] == '=') {
        value[1] = '=';
        lexer->pos++;
        lexer->col++;
        strcpy(token.type, "relop");
        strcpy(token.value, "!=");
        return token;
    }
    
    // Modify these for language-specific operators
    if (current == '+' || current == '-') strcpy(token.type, "addop");
    else if (current == '*' || current == '/' || current == '%') strcpy(token.type, "mulop");
    else strcpy(token.type, value); // Single-character tokens like (, ), {, }, etc.
    //{} () etc are used in grammar but have no specific non-terminal so no specific token type
    strcpy(token.value, value);
    return token;
}