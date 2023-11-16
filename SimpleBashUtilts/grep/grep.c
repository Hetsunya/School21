#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre.h>

#define MAX_LINE_LENGTH 4096

#define FLAG_IGNORE_CASE 1
#define FLAG_INVERT_MATCH 2
#define FLAG_COUNT_ONLY 4
#define FLAG_MATCHING_FILES_ONLY 8
#define FLAG_DISPLAY_LINE_NUMBER 16
#define FLAG_SUPPRESS_ERRORS 32
#define FLAG_FROM_FILE 64
#define FLAG_ONLY_MATCHING_PARTS 128

int flags = 0;

void display_usage() {
    printf("Usage: s21_grep [options] pattern [file...]\n");
    printf("Options:\n");
    printf("  -e pattern              use pattern as the expression to match\n");
    printf("  -i                      ignore case distinctions in both the PATTERN and the input files\n");
    printf("  -v                      select non-matching lines\n");
    printf("  -c                      print only a count of matching lines per FILE\n");
    printf("  -l                      print only names of FILEs containing matches\n");
    printf("  -n                      print line number with output lines\n");
    printf("  -h                      suppress the prefixing of file names on output\n");
    printf("  -s                      suppress error messages about nonexistent or unreadable files\n");
    printf("  -f file                 read patterns from file\n");
    printf("  -o                      print only the matched (non-empty) parts of a matching line\n");
}

char *read_pattern_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening pattern file");
        exit(EXIT_FAILURE);
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate a buffer for reading the file
    char *buffer = (char *)malloc(size + 1);
    if (!buffer) {
        perror("Memory allocation error");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fread(buffer, 1, size, file);
    fclose(file);

    buffer[size] = '\0';

    return buffer;
}

int parse_flags(int argc, char *argv[], char **pattern) {
    *pattern = NULL;
    
    int no_flag = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            no_flag++;
            if (argv[i][1] == 'f') {
                // Handle -f separately
                if (argv[i][2] != '\0') {
                    // Use the rest of the current argument as the file name
                    *pattern = read_pattern_from_file(argv[i] + 2);
                    // printf("%s", *pattern);
                    flags |= FLAG_FROM_FILE;
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    // Use the next argument as the file name
                    *pattern = read_pattern_from_file(argv[++i]);
                    // printf("%s", *pattern);
                    flags |= FLAG_FROM_FILE;
                } else {
                    printf("Option -f requires a file argument.\n");
                    display_usage();
                    exit(EXIT_FAILURE);
                }
            } else {
                // Handle other options
                int j = 1;
                while (argv[i][j] != '\0') {
                    switch (argv[i][j]) {
                        case 'e':
                            if (i + 1 < argc) {
                                *pattern = argv[++i];
                                flags |= FLAG_FROM_FILE;//HZ FIX IT 
                            } else {
                                printf("Option -e requires an argument.\n");
                                display_usage();
                                exit(EXIT_FAILURE);
                            }
                            break;
                        case 'i':
                            flags |= FLAG_IGNORE_CASE;
                            break;
                        case 'v':
                            flags |= FLAG_INVERT_MATCH;
                            break;
                        case 'c':
                            flags |= FLAG_COUNT_ONLY;
                            break;
                        case 'l':
                            flags |= FLAG_MATCHING_FILES_ONLY;
                            break;
                        case 'n':
                            flags |= FLAG_DISPLAY_LINE_NUMBER;
                            break;
                        case 'h':
                            flags |= FLAG_SUPPRESS_ERRORS;
                            break;
                        case 's':
                            flags |= FLAG_SUPPRESS_ERRORS;
                            break;
                        case 'o':
                            flags |= FLAG_ONLY_MATCHING_PARTS;
                            break;
                        default:
                            printf("Unknown option: -%c\n", argv[i][j]);
                            display_usage();
                            exit(EXIT_FAILURE);
                    }
                    j++;
                }
            }
        }
        
    }
    if (no_flag == 0){
        *pattern = argv[1];
    }


    return argc; // No pattern found
}

void grep_file(char *filename, char *pattern) {
    // printf("%s greping\n", pattern);
    FILE *file = fopen(filename, "r");
    if (!file) {
        if (!(flags & FLAG_SUPPRESS_ERRORS)) {
            // printf("grep: %s: ", filename);
            perror("Error opening file");
        }
        return;
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 1;
    int match_count = 1;
    pcre *re;

    const char *error;
    int erroffset;
    int options = 0;

    if (flags & FLAG_IGNORE_CASE) {
        options |= PCRE_CASELESS;
    }

    if (flags & FLAG_FROM_FILE) {
        // Use the pattern read from the file
        re = pcre_compile(pattern, options, &error, &erroffset, NULL);
    } else {
        // Use the pattern directly
        re = pcre_compile(pattern, options, &error, &erroffset, NULL);
    }
    // printf("%s = re", re);
    // printf("%s = file", filename);
    while (fgets(line, sizeof(line), file) != NULL) {
        int match;
        int ovector[30];
        int rc = pcre_exec(re, NULL, line, strlen(line), 0, 0, ovector, sizeof(ovector) / sizeof(int));
        // printf("%d = rc\n", rc);

        if (flags & FLAG_INVERT_MATCH) {
            match = (rc == PCRE_ERROR_NOMATCH);
        } else {
            match = (rc >= 0);
        }
        // printf("%d = match\n", match);

        if (match) {
            match_count++;

            if (flags & FLAG_COUNT_ONLY) {
                break;
            } else if (flags & FLAG_MATCHING_FILES_ONLY) {
                if (flags & FLAG_SUPPRESS_ERRORS) {
                    printf("%s\n", filename);
                    fclose(file);
                    return;
                } else {
                    printf("%s\n", filename);
                    break;
                }
            } else if (flags & FLAG_DISPLAY_LINE_NUMBER) {
                printf("%d:", line_number);
            }

            if (flags & FLAG_ONLY_MATCHING_PARTS) {
                int start = ovector[0];
                int end = ovector[1];
                printf("%.*s\n", end - start, line + start);
            } else {
                printf("%s", line);
            }
        }

        line_number++;        // printf("konec");
    }

    if (flags & FLAG_COUNT_ONLY) {
        printf("%d\n", match_count);
    }

    fclose(file);
    pcre_free(re);
}

int main(int argc, char *argv[]) {
    char *pattern;
    int file_arg_start = parse_flags(argc, argv, &pattern);
    // printf("%s posle pars\n", pattern);
    if (!pattern) {
        printf("Pattern not specified.\n");
        display_usage();
        return EXIT_FAILURE;
    }
if (flags & FLAG_FROM_FILE) {
    for (int i = 1; i < argc; i++) {
        if (argv[i - 1][0] == '-' && argv[i - 1][1] == 'f') {
            continue;
        }
        else if (argv[i][0] != '-') {
            int flag_exit = 1;

            for (int j = 0; argv[i][j] != '\0'; j++) {
                if (argv[i][j] == '-' && (j == 0 || argv[i][j + 1] == 'f' || (i > 0 && argv[i - 1][0] == '-' && argv[i - 1][1] == 'f'))) {
                    flag_exit = 0;

                    // Если флаг '-f' обнаружен, пропускаем следующий аргумент
                    if (argv[i][j + 1] == 'f' && argv[i][j + 2] != '\0') {
                        i++;
                    }
                    break;
                }
            }

            // printf("%d = flag\n", flag_exit);

            if (!flag_exit) {
                continue;
            } else {
                printf("%s на вход\n", argv[i]);
                grep_file(argv[i], pattern);
            }
        }
    }
}


    else if (file_arg_start == argc) {
        // No file provided, read from stdin
        grep_file("/dev/stdin", pattern);
                        
    } else {
        // Display each specified file
        for (int i = file_arg_start; i < argc; i++) {
        grep_file(argv[i], pattern);
        }
    }
    // printf("%d = file_arg_start", file_arg_start);

    return EXIT_SUCCESS;
}
