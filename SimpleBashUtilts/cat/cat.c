#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cat.h"

int flags = 0;

void display_usage() {
    printf("Usage: s21_cat [options] [file...]\n");
    printf("Options:\n");
    printf("  -b, --number-nonblank    numbers only non-empty lines\n");
    printf("  -e                      implies -v but also display end-of-line characters as $\n");
    printf("  -n, --number            number all output lines\n");
    printf("  -s, --squeeze-blank    squeeze multiple adjacent blank lines\n");
    printf("  -t                      implies -v but also display tabs as ^I\n");
}

int parse_flags(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--number-nonblank") == 0) {
                flags |= FLAG_NUMBER_NONBLANK;
            } else if (strcmp(argv[i], "-e") == 0) {
                flags |= (FLAG_DISPLAY_EOL | FLAG_NUMBER_NONBLANK);
            } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--number") == 0) {
                flags |= FLAG_NUMBER_LINES;
            } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--squeeze-blank") == 0) {
                flags |= FLAG_SQUEEZE_BLANK;
            } else if (strcmp(argv[i], "-t") == 0) {
                flags |= (FLAG_DISPLAY_TABS | FLAG_NUMBER_NONBLANK);
            } else {
                printf("Unknown option: %s\n", argv[i]);
                display_usage();
                exit(EXIT_FAILURE);
            }
        } else {
            // File argument found, stop processing options
            return i;
        }
    }
    return argc;
}

void display_file(char *filename, int flags) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[1024];
    int line_number = 0;
    int prev_blank = 0;

    while (fgets(line, sizeof(line), file)) {
        if (flags & FLAG_SQUEEZE_BLANK) {
            if (line[0] == '\n') {
                if (prev_blank) {
                    continue;  // Skip multiple adjacent blank lines
                }
                prev_blank = 1;
            } else {
                prev_blank = 0;
            }
        }

        switch (flags) {
            case FLAG_DISPLAY_TABS | FLAG_NUMBER_NONBLANK | FLAG_DISPLAY_EOL:
                for (int i = 0; line[i] != '\0'; i++) {
                    if (line[i] == '\t') {
                        printf("%d^I", line_number);
                    } else {
                        putchar(line[i]);
                    }
                }
                printf("$");
                break;

            case FLAG_DISPLAY_TABS | FLAG_NUMBER_NONBLANK:
                for (int i = 0; line[i] != '\0'; i++) {
                    if (line[i] == '\t') {
                        printf("%d^I", line_number);
                    } else {
                        putchar(line[i]);
                    }
                }
                break;

            // Add cases for other flag combinations as needed

            default:
                if (flags & FLAG_NUMBER_NONBLANK) {
                    printf("%d\t", line_number);
                }
                fputs(line, stdout);
                if (flags & FLAG_DISPLAY_EOL) {
                    printf("$");
                }
                break;
        }

        line_number++;
    }

    fclose(file);
}


int main(int argc, char *argv[]) {
    int file_arg_start = parse_flags(argc, argv);

    if (file_arg_start == argc) {
        // No file provided, read from stdin
        display_file("/dev/stdin", flags);
    } else {
        // Display each specified file
        for (int i = file_arg_start; i < argc; i++) {
            display_file(argv[i], flags);
        }
    }

    return 0;
}
