#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024

// Helper function: trim leading and trailing whitespace in place
void trim(char *str) {
    int i, start = 0, end = (int)strlen(str) - 1;
    while (isspace((unsigned char)str[start])) start++;
    while (end >= start && isspace((unsigned char)str[end])) end--;
    for (i = start; i <= end; i++) {
        str[i - start] = str[i];
    }
    str[i - start] = '\0';
}

// Check if the line might be a function declaration/definition.
// This is a simplistic check: the line must contain both '(' and ')'
// and must not end with a ';' (which would indicate a prototype).
int is_function_line(const char *line) {
    return (strchr(line, '(') != NULL &&
            strchr(line, ')') != NULL &&
            strchr(line, ';') == NULL);
}

int main(void) {
    FILE *fin = fopen("src.c", "r");
    if (!fin) {
        fprintf(stderr, "Error opening input file\n");
        return 1;
    }

    char line[MAX_LINE];
    int index = 0, ch;

    // Read the file character by character using fgetc() and build lines.
    while ((ch = fgetc(fin)) != EOF) {
        if (ch != '\n') {
            if (index < MAX_LINE - 1) {
                line[index++] = (char)ch;
            }
        } else {
            line[index] = '\0';
            index = 0;

            // Process only lines that look like a function header
            if (is_function_line(line)) {
                // Find the first '(' in the line
                char *openParen = strchr(line, '(');
                if (!openParen) continue; // should not happen

                // Extract the parameter substring
                char params[256] = "";
                char *closeParen = strchr(openParen, ')');
                if (closeParen && closeParen > openParen + 1) {
                    int paramLen = (int)(closeParen - openParen - 1);
                    strncpy(params, openParen + 1, paramLen);//copy from openParen+1 means the one right after ( to params till paramLen
                    params[paramLen] = '\0';
                    trim(params);
                }

                // Count arguments (if any)
                int argCount = 0;
                if (strlen(params) == 0 || strcmp(params, "void") == 0) {//basically if its like int main(void)
                    argCount = 0;
                } else {
                    argCount = 1;
                    for (int i = 0; params[i]; i++) {
                        if (params[i] == ',')
                            argCount++;
                    }
                }

                // Find the function name:
                // Step backward from openParen, skipping spaces.
                char *p = openParen - 1;
                while (p >= line && isspace((unsigned char)*p)) {
                    p--;
                }
                // p now points to the last character of the function name.
                char *funcEnd = p + 1;
                // Find the start of the function name.
                while (p >= line && (isalnum((unsigned char)*p) || *p == '_')) {
                    p--;
                }
                char *funcStart = p + 1;
                char functionName[256] = "";
                int len = (int)(funcEnd - funcStart);
                strncpy(functionName, funcStart, len);
                functionName[len] = '\0';

                // Determine the return type.
                // We'll take the substring from the start of the line up to the function name.
                char returnType[256] = "";
                int pos = (int)(funcStart - line);
                if (pos > 0) {
                    char temp[256] = "";
                    strncpy(temp, line, pos);
                    temp[pos] = '\0';
                    trim(temp);
                    // For simplicity, assume the last token in temp is the return type.
                    // (This may not be perfect for all cases, e.g., for "static unsigned int".)
                    char *token = strtok(temp, " \t");
                    char lastToken[256] = "";
                    while (token != NULL) {
                        strcpy(lastToken, token);
                        token = strtok(NULL, " \t");
                    }
                    strcpy(returnType, lastToken);
                } else {
                    strcpy(returnType, "unknown");
                }

                // Display the result
                fprintf(stdout, "Function: %s, Return Type: %s, Number of Arguments: %d\n",
                        functionName, returnType, argCount);
            }
        }
    }
    // Process any leftover line (if file doesn't end with a newline)
    if (index > 0) {
        line[index] = '\0';
        if (is_function_line(line)) {
            // Process similarly as above (omitted here for brevity)
        }
    }

    fclose(fin);
    return 0;
}
