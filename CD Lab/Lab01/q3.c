#include <stdio.h>
#include <stdlib.h>

int main(void) {
    FILE* in1 = fopen("input1.c", "r");
    FILE* in2 = fopen("output2.c", "r");
    FILE* out = fopen("output3.txt", "w");
    if (in1 == NULL || in2 == NULL || out == NULL) {
        perror("Error opening one of the files");
        if(in1) fclose(in1);
        if(in2) fclose(in2);
        if(out) fclose(out);
        return EXIT_FAILURE;
    }
    
    int c1 = fgetc(in1);  // Initialize c1
    int c2 = fgetc(in2);  // Initialize c2

    // Continue looping until both files are at EOF.
    while(c1 != EOF || c2 != EOF) {
        // Process one line from file in1
        while(c1 != EOF && c1 != '\n') {
            fputc(c1, out);
            c1 = fgetc(in1);
        }
        // Write newline if it was encountered in in1
        if(c1 == '\n') {
            fputc(c1, out);
            c1 = fgetc(in1);
        }
        
        // Process one line from file in2
        while(c2 != EOF && c2 != '\n') {
            fputc(c2, out);
            c2 = fgetc(in2);
        }
        // Write newline if it was encountered in in2
        if(c2 == '\n') {
            fputc(c2, out);
            c2 = fgetc(in2);
        }
    }
    
    fclose(in1);
    fclose(in2);
    fclose(out);
    return 0;
}
