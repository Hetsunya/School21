#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

void grep(const char *search_term, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, search_term) != NULL) {
            printf("%s", line);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <search_term> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *search_term = argv[1];
    const char *filename = argv[2];

    grep(search_term, filename);

    return 0;
}
