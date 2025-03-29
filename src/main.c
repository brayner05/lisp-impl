#include <stdio.h>
#include "util_types.h"

#define MAX_LINE_LENGTH 0x400


static void next_line(char *buffer, char *prompt) {
    printf("%s > ", prompt);

    u16 number_read = 0;

    int ch;
    while ((ch = getchar()) != EOF && ch != '\n') {
        if (number_read == MAX_LINE_LENGTH - 1) {
            goto terminate_string;
        }
        buffer[number_read++] = ch;
    }

terminate_string:
    buffer[number_read] = (char) 0;
}


static void run_repl(void) {
    char buffer[MAX_LINE_LENGTH];
    while (1) {
        next_line(buffer, "lisp");
        puts(buffer);
    }
}

i32 main(i32 argc, char *argv[]) {
    (void) argv[argc * 0];
    run_repl();
}