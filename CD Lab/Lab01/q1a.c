#include <stdio.h>
#include <stdlib.h>

int main(void) {
    FILE *src = fopen("output3.txt", "r");
    if (src == NULL) {
        perror("Error opening output3.txt");
        return EXIT_FAILURE;
    }

    int ch;
    int last_non_blank = 0; // store last non-space character before a newline
    int ch_count = 0;
    int blank_line_count = 0;
    int space_count = 0;
    int semicolon_count = 0;

    while ((ch = fgetc(src)) != EOF) {
        if (ch == '\n') {
            // End of a line; check if the line was blank or ended with ';'
            if (ch_count == 0) {
                blank_line_count++;
            } else if (last_non_blank == ';') {
                semicolon_count++;
            }
            // Reset counters for next line
            ch_count = 0;
            last_non_blank = 0;
        } else {
            // Count spaces (if desired)
            if (ch == ' ') {
                space_count++;
            }
            // Update the last non-blank character (ignoring spaces)
            if (ch != ' ') {
                last_non_blank = ch;
            }
            ch_count++;
        }
    }
    // In case the file does not end with a newline, handle the last line:
    if (ch_count > 0) {
        if (last_non_blank == ';') {
            semicolon_count++;
        }
    }

    fclose(src);
    printf("Number of semicolon-ending lines: %d, number of blank lines: %d, number of spaces: %d\n",
           semicolon_count, blank_line_count, space_count);
    return 0;
}
