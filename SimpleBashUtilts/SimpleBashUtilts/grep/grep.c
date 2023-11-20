#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 4096
#define MAX_PATTERNS 100

#define FLAG_IGNORE_CASE 1
#define FLAG_INVERT_MATCH 2
#define FLAG_COUNT_ONLY 4
#define FLAG_MATCHING_FILES_ONLY 8
#define FLAG_DISPLAY_LINE_NUMBER 16
#define FLAG_SUPPRESS_ERRORS 32
#define FLAG_FROM_FILE 64
#define FLAG_ONLY_MATCHING_PARTS 128
#define FLAG_EXTENDED_PATTERN 256
#define FLAG_NO_NAMES 512

char *patters[MAX_PATTERNS];
int flags = 0;

void display_usage() {
  printf("Usage: s21_grep [options] pattern [file...]\n");
  printf("Options:\n");
  printf(
      "  -e pattern              use pattern as the expression to"
      "match\n");
  printf(
      "  -i                      ignore case distinctions in both the PATTERN "
      "and the input files\n");
  printf("  -v                      select non-matching lines\n");
  printf(
      "  -c                      print only a count of matching lines per "
      "FILE\n");
  printf(
      "  -l                      print only names of FILEs containing "
      "matches\n");
  printf("  -n                      print line number with output lines\n");
  printf(
      "  -h                      suppress the prefixing of file names on "
      "output\n");
  printf(
      "  -s                      suppress error messages about nonexistent or "
      "unreadable files\n");
  printf("  -f file                 read patterns from file\n");
  printf(
      "  -o                      print only the matched (non-empty) parts of a "
      "matching line\n");
}

char *read_pattern_from_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening pattern file");
    exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = (char *)malloc(size + 1);
  if (!buffer) {
    perror("Memory allocation error");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  fread(buffer, 1, size, file);
  fclose(file);

  if (size == 0) {
    fprintf(stderr, "Error: Pattern file is empty.\n");
    exit(EXIT_FAILURE);
  }

  while (size > 0 && (buffer[size - 1] == '\n' || buffer[size - 1] == '\r')) {
    size--;
  }

  buffer[size] = '\0';

  return buffer;
}

int parse_flags(int argc, char *argv[], char **pattern, int *pattern_count) {
  *pattern = NULL;
  int no_flag = 0;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      no_flag++;
      if (argv[i][1] == 'f') {
        if (argv[i][2] != '\0') {
          *pattern = read_pattern_from_file(argv[i] + 2);
          flags |= FLAG_FROM_FILE;
        } else if (i + 1 < argc && argv[i + 1][0] != '-') {
          *pattern = read_pattern_from_file(argv[++i]);
          flags |= FLAG_FROM_FILE;
        } else {
          printf("Option -f requires a file argument.\n");
          display_usage();
          exit(EXIT_FAILURE);
        }
      } else if (argv[i][1] == 'e') {
        if (argv[i][2] != '\0') {
          *patters[*pattern_count] = (argv[i] + 2);
          pattern_count++;
          printf("%s", *pattern);
        } else if (i + 1 < argc && argv[i + 1][0] != '-') {
            printf("%d\n", pattern_count);
          *patters[*pattern_count] = (argv[++i]);
          pattern_count++;
          printf("%s", *pattern);
        } else {
          printf("Option -e requires pattern argument.\n");
          display_usage();
          exit(EXIT_FAILURE);
        }

        if (!(flags & FLAG_EXTENDED_PATTERN)) {
          flags |= FLAG_EXTENDED_PATTERN;
        }
      } else {
        int j = 1;
        while (argv[i][j] != '\0') {
          switch (argv[i][j]) {
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
              flags |= FLAG_NO_NAMES;
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
    } else if ((!(flags & FLAG_FROM_FILE) &&
                !(flags & FLAG_EXTENDED_PATTERN)) &&
               *pattern == NULL) {
      *pattern = argv[i];
    }
  }
  if (no_flag == 0) {
    *pattern = argv[1];
  }

  return 0;
}

void grep_file(char *filename, char *pattern, int file_count) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    if (!(flags & FLAG_SUPPRESS_ERRORS)) {
      perror("Error opening file");
    }
    return;
  }

  char line[MAX_LINE_LENGTH];
  int line_number = 1;
  int match_count = 0;
  pcre *re;

  const char *error;
  int erroffset;
  int options = 0;

  if (flags & FLAG_IGNORE_CASE) {
    options |= PCRE_CASELESS;
  }

  if (flags & FLAG_FROM_FILE) {
    re = pcre_compile(pattern, options, &error, &erroffset, NULL);
  } else {
    re = pcre_compile(pattern, options, &error, &erroffset, NULL);
  }
  while (fgets(line, sizeof(line), file) != NULL) {
    int match;
    int ovector[30];
    int rc = pcre_exec(re, NULL, line, strlen(line), 0, 0, ovector,
                       sizeof(ovector) / sizeof(int));

    if (flags & FLAG_INVERT_MATCH) {
      match = (rc == PCRE_ERROR_NOMATCH);
    } else {
      match = (rc >= 0);
    }
    if (match) {
      match_count++;
      if (flags & FLAG_COUNT_ONLY) {
        continue;
      } else if (flags & FLAG_MATCHING_FILES_ONLY) {
        printf("%s\n", filename);
        fclose(file);
        return;
      }
      if (file_count > 1 && !(flags & FLAG_NO_NAMES)) {
        printf("%s:", filename);
        if (flags & FLAG_DISPLAY_LINE_NUMBER) {
          printf("%d:", line_number);
        }
        if (flags & FLAG_ONLY_MATCHING_PARTS) {
          int start = ovector[0];
          int end = ovector[1];
          printf("%.*s\n", end - start, line + start);
        } else {
          printf("%s", line);
        }
      } else {
        if (flags & FLAG_DISPLAY_LINE_NUMBER) {
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
    }
    line_number++;
  }

  if ((flags & FLAG_COUNT_ONLY)) {
    if (file_count > 1 && !(flags & FLAG_NO_NAMES)) {
      printf("%s:", filename);
    }
    printf("%d\n", match_count);
  }

  fclose(file);
  pcre_free(re);
}

int count_files(int argc, char *argv[], char *pattern) {
  int file_count = 0;
  for (int i = 1; i < argc; i++) {
    if (((argv[i][0] == '-' && argv[i][1] == 'f') ||
         (argv[i - 1][0] == '-' && argv[i - 1][1] == 'f')) ||
        ((argv[i][0] == '-' && argv[i][1] == 'e') ||
         (argv[i - 1][0] == '-' && argv[i - 1][1] == 'e')) ||
        argv[i][0] == '-' || (pattern == argv[i])) {
      continue;
    }
    file_count++;
  }
  return file_count;
}

void pars_for_grep(int argc, char *argv[], char *pattern) {
  int file_count = count_files(argc, argv, pattern);

  for (int i = 1; i < argc; i++) {
    if (pattern == argv[i]) {
      continue;
    }
    if (flags & FLAG_FROM_FILE) {
      if (argv[i - 1][0] == '-' && argv[i - 1][1] == 'f') {
        continue;
      } else if (argv[i][0] != '-') {
        int flag_exit = 1;

        for (int j = 0; argv[i][j] != '\0'; j++) {
          if (argv[i][j] == '-' &&
              (j == 0 || argv[i][j + 1] == 'f' ||
               (i > 0 && argv[i - 1][0] == '-' && argv[i - 1][1] == 'f'))) {
            flag_exit = 0;
            if (argv[i][j + 1] == 'f' && argv[i][j + 2] != '\0') {
              i++;
            }
            break;
          }
        }

        if (!flag_exit) {
          continue;
        } else {
          grep_file(argv[i], pattern, file_count);
        }
      }
    } else {
      if (argv[i][0] == '-') {
        continue;
      }
      grep_file(argv[i], pattern, file_count);
    }
  }
}

int main(int argc, char *argv[]) {
    char *pattern;
    int *pattern_count = 0;
    parse_flags(argc, argv, &pattern, pattern_count);

    for (int i = 0; i < *pattern_count; i++){
        printf("%s\n",patters[i]);
    }

    if (!pattern) {
      printf("Pattern not specified.\n");
      display_usage();
      return EXIT_FAILURE;
    }
    pars_for_grep(argc, argv, pattern);

  return EXIT_SUCCESS;
}
