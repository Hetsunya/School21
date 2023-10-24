#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage() {
    printf("Usage: s21_grep [-e pattern] [-i] [-v] [-c] [-l] [-n] [file ...]\n");
}

int str_contains(const char *str, const char *pattern) {
    return strstr(str, pattern) != NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    char *pattern = NULL;
    int case_insensitive = 0;
    int invert_match = 0;
    int count_only = 0;
    int print_filename = 0;
    int print_line_number = 0;

    // Парсинг аргументов командной строки
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
            pattern = argv[++i];
        } else if (strcmp(argv[i], "-i") == 0) {
            case_insensitive = 1;
        } else if (strcmp(argv[i], "-v") == 0) {
            invert_match = 1;
        } else if (strcmp(argv[i], "-c") == 0) {
            count_only = 1;
        } else if (strcmp(argv[i], "-l") == 0) {
            print_filename = 1;
        } else if (strcmp(argv[i], "-n") == 0) {
            print_line_number = 1;
        } else {
            // Обработка файла
            FILE *file = fopen(argv[i], "r");
            if (file == NULL) {
                perror("Error opening file");
                return 1;
            }

            char *line = NULL;
            size_t len = 0;
            int line_number = 0;
            int match_count = 0;

            // Обработка каждой строки файла
            while (getline(&line, &len, file) != -1) {
                line_number++;

                int match = str_contains(line, pattern);

                if ((match && !invert_match) || (!match && invert_match)) {
                    if (count_only) {
                        match_count++;
                    } else {
                        if (print_filename) {
                            printf("%s\n", argv[i]);
                            break; // прерываем после первого совпадения для -l
                        }

                        if (print_line_number) {
                            printf("%d:", line_number);
                        }

                        printf("%s", line);
                    }
                }
            }

            if (count_only) {
                printf("%d\n", match_count);
            }

            fclose(file);
            free(line);
        }
    }

    return 0;
}
    