#include "la.h"
typedef struct{
    Token currToken;
    Lexer lexer;
    int errorcount;
}Parser;

void advance(Parser* parser){
    parser->currToken= getNextToken(&parser->lexer);
}
int match(Parser* parser, const char*compareToken){
    if(strcmp(parser->currToken.type,compareToken)==0){
        advance(parser);
        return 1;
    }
    return 0;
}
void error(Parser* parser,const char* expected){
    printf("Error at row %d and col %d: expected %s got %s\n",parser->currToken.row,parser->currToken.col,expected,parser->currToken.value);
    parser->errorcount++;
}

void Parser_init(Parser* parser,const char* source){
    Lexer_init(&parser->lexer,source);
    parser->errorcount=0;
    advance(parser);

}

void Program(Parser* parser);
void declarations(Parser* parser);
void data_type(Parser* parser);
void identifier_list(Parser* parser);
void identifier_list_prime(Parser* parser);
void assign_stat(Parser* parser);
void assign_stat_prime(Parser* parser);

void Program(Parser* parser){
    if(!match(parser,"main")) error(parser,"main");
    if(!match(parser,"("))error(parser,"(");
    if(!match(parser,")"))error(parser,")");
    if(!match(parser,"{"))error(parser,"{");
    declarations(parser);
    assign_stat(parser);
    if(!match(parser,"}"))error(parser,"}");
    
    if (strcmp(parser->currToken.type, "EOF") == 0 && parser->errorcount == 0) {
        printf("Parsing was successful\n");
    }
    printf("Parsing completed with %d error(s)\n", parser->errorcount);
}
void declarations(Parser* parser){
    if(strcmp(parser->currToken.type,"int")==0||strcmp(parser->currToken.type,"char")==0){//check FIRST values
        data_type(parser);
        identifier_list(parser);
        if(!match(parser,";"))error(parser,";");
        declarations(parser);
    }
}
void data_type(Parser* parser){
    if (!(match(parser, "int") || match(parser, "char"))) error(parser,"int or char");
}
void identifier_list(Parser* parser){
    if(!match(parser,"id")) error(parser,"id");
    identifier_list_prime(parser);
}
void identifier_list_prime(Parser* parser){//here it has ε case so if not , i cannot give error
    if(match(parser,",")){
        identifier_list(parser);
    }
}
void assign_stat(Parser* parser){
    if(!match(parser,"id")) error(parser,"id");
    if(!match(parser,"=")) error(parser,"=");
    assign_stat_prime(parser);
}
void assign_stat_prime(Parser* parser){
    if(match(parser,"id")){
        if(!match(parser,";"))error(parser,";");
    }
    else if(match(parser,"num")){
        if(!match(parser,";"))error(parser,";");
    }
    else{
        error(parser,"id or num");
    }
}

char* read_file(const char* filename){
    FILE* file= fopen(filename,"r");
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
int main(int argc, char* argv[]){
    if(argc!=2){
        perror("Input file required\n");
        exit(EXIT_FAILURE);
    }
    char* input= read_file(argv[1]);
    if (input == NULL) {
        return 1;
    }
    printf("Parsing file: %s\n", argv[1]);
    Parser parser;
    Parser_init(&parser, input);
    Program(&parser);

    free(input);
    return 0;
}