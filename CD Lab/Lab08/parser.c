#include "la.h"
#include <stdlib.h>

//###THE PLACES WHERE FIRST of a production is used only where there is no predefined token.type which is also used
//in our LHS side of code...eg: statement_list → statement statement_list | ε
//overe here we don't have any token.type==statement and we have a | ε so an if is required#####

// Parser structure - Generic, language-independent
typedef struct {
    Lexer lexer;         // Lexer instance
    Token currentToken;  // Current token being processed
    int errorCount;      // Error counter
} Parser;

// Parser function declarations - Mostly grammar-specific
void advance(Parser* parser);
void error(Parser* parser, const char* expected);
int match(Parser* parser, const char* type);
void parser_init(Parser* parser, const char* source);

void program(Parser* parser);
void declarations(Parser* parser);
void data_type(Parser* parser);
void identifier_list(Parser* parser);
void identifier_list_prime(Parser* parser);
void identifier_list_double_prime(Parser* parser);
void statement_list(Parser* parser);
void statement(Parser* parser);
void assign_stat(Parser* parser);
void expn(Parser* parser);
void eprime(Parser* parser);
void simple_expn(Parser* parser);
void seprime(Parser* parser);
void term(Parser* parser);
void tprime(Parser* parser);
void factor(Parser* parser);
void decision_stat(Parser* parser);
void dprime(Parser* parser);

// Generic parser utilities - Language-independent
void advance(Parser* parser) {
    parser->currentToken = getNextToken(&parser->lexer);
}

void error(Parser* parser, const char* expected) {
    printf("Error at row %d, column %d: expected %s, found '%s'\n",
           parser->currentToken.row, parser->currentToken.col,
           expected, parser->currentToken.value);
    parser->errorCount++;
}

int match(Parser* parser, const char* type) {
    if (strcmp(parser->currentToken.type, type) == 0) {//match is with token type in case we have id or num
        advance(parser);
        return 1;
    }
    return 0;
}

void parser_init(Parser* parser, const char* source) {
    lexer_init(&parser->lexer, source);
    parser->errorCount = 0;
    advance(parser);//basically get a token after initialization
}

/* 
 * Language-specific: Modify this function based on the grammar's start rule
 * For C: Program → main () { declarations statement-list }
 */
void program(Parser* parser) {
    if (!match(parser, "main")) error(parser, "main");
    if (!match(parser, "(")) error(parser, "(");
    if (!match(parser, ")")) error(parser, ")");
    if (!match(parser, "{")) error(parser, "{");
    declarations(parser);
    statement_list(parser);
    if (!match(parser, "}")) error(parser, "}");
    
    if (strcmp(parser->currentToken.type, "EOF") == 0 && parser->errorCount == 0) {
        printf("Parsing was successful\n");
    }
    printf("Parsing completed with %d error(s)\n", parser->errorCount);
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: declarations → data-type identifier-list; declarations | ε
 */
void declarations(Parser* parser) {
    if (strcmp(parser->currentToken.type, "int") == 0 || 
        strcmp(parser->currentToken.type, "char") == 0) {
        data_type(parser);//actually doesn't really make sense coz we already compared but ok
        identifier_list(parser);
        if (!match(parser, ";")) error(parser, ";");
        declarations(parser);
    }
    // ε case handled implicitly by doing nothing if no data-type is found
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: data-type → int | char
 */
void data_type(Parser* parser) {
    if (!(match(parser, "int") || match(parser, "char"))) {//actually doesn't really make sense coz we already compared but ok
        error(parser, "int or char");
    }
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: identifier-list → id identifier-list'
 */
void identifier_list(Parser* parser) {
    if (!match(parser, "id")) error(parser, "identifier");
    identifier_list_prime(parser);
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: identifier-list' → , identifier-list | [number] identifier-list'' | ε
 */
void identifier_list_prime(Parser* parser) {
    if (match(parser, ",")) {
        identifier_list(parser);
    }
    else if (match(parser, "[")) {
        if (!match(parser, "num")) error(parser, "number");
        if (!match(parser, "]")) error(parser, "]");
        identifier_list_double_prime(parser);
    }
    // ε case handled implicitly
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: identifier-list'' → , identifier-list | ε
 */
void identifier_list_double_prime(Parser* parser) {
    if (match(parser, ",")) {
        identifier_list(parser);
    }
    // ε case handled implicitly
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: statement_list → statement statement_list | ε
 */
void statement_list(Parser* parser) {
    //FIRST only used coz it has ε for which an if condition is required
    if (strcmp(parser->currentToken.type, "id") == 0 || //these are first values of statement_list which it gets from statement
        strcmp(parser->currentToken.type, "if") == 0) {
        statement(parser);
        statement_list(parser);
    }
    // ε case handled implicitly
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: statement → assign-stat; | decision-stat
 */
void statement(Parser* parser) {
    if (strcmp(parser->currentToken.type, "id") == 0) {//this is the first value which it gets from assign_stat
        assign_stat(parser);
        if (!match(parser, ";")) error(parser, ";");
    }
    else if (strcmp(parser->currentToken.type, "if") == 0) {//this is the first value which it gets from decision_stat
        decision_stat(parser);
    }
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: assign-stat → id = expn
 */
void assign_stat(Parser* parser) {
    if (!match(parser, "id")) error(parser, "identifier");
    if (!match(parser, "=")) error(parser, "=");
    expn(parser);
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: expn → simple-expn eprime
 */
void expn(Parser* parser) {
    simple_expn(parser);
    eprime(parser);
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: eprime → relop simple-expn | ε
 */
void eprime(Parser* parser) {
    if (strcmp(parser->currentToken.type, "relop") == 0) {//could've use match(parser,"relop")
        advance(parser);
        simple_expn(parser);
    }
    // ε case handled implicitly
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: simple-expn → term seprime
 */
void simple_expn(Parser* parser) {
    term(parser);
    seprime(parser);
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: seprime → addop term seprime | ε
 */
void seprime(Parser* parser) {
    if (strcmp(parser->currentToken.type, "addop") == 0) {
        advance(parser);
        term(parser);
        seprime(parser);
    }
    // ε case handled implicitly
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: term → factor tprime
 */
void term(Parser* parser) {
    factor(parser);
    tprime(parser);
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: tprime → mulop factor tprime | ε
 */
void tprime(Parser* parser) {
    if (strcmp(parser->currentToken.type, "mulop") == 0) {
        advance(parser);
        factor(parser);
        tprime(parser);
    }
    // ε case handled implicitly
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: factor → id | num
 */
void factor(Parser* parser) {
    if (!(match(parser, "id") || match(parser, "num"))) {
        error(parser, "identifier or number");
    }
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: decision-stat → if (expn) { statement_list } dprime
 */
void decision_stat(Parser* parser) {
    if (!match(parser, "if")) error(parser, "if");
    if (!match(parser, "(")) error(parser, "(");
    expn(parser);
    if (!match(parser, ")")) error(parser, ")");
    if (!match(parser, "{")) error(parser, "{");
    statement_list(parser);
    if (!match(parser, "}")) error(parser, "}");
    dprime(parser);
}

/* 
 * Language-specific: Modify based on grammar rule
 * For C: dprime → else { statement_list } | ε
 */
void dprime(Parser* parser) {
    if (match(parser, "else")) {
        if (!match(parser, "{")) error(parser, "{");
        statement_list(parser);
        if (!match(parser, "}")) error(parser, "}");
    }
    // ε case handled implicitly
}

// File reading utility - Language-independent
char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, size, file);
    buffer[read_size] = '\0';

    fclose(file);
    return buffer;
}

// Main function - Language-independent
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    char* input = read_file(argv[1]);
    if (input == NULL) {
        return 1;
    }

    printf("Parsing file: %s\n", argv[1]);
    Parser parser;
    parser_init(&parser, input);
    program(&parser);

    free(input);
    return 0;
}