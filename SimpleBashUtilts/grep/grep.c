#include <pcre.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 4096

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

int flags = 0;

void display_usage() {
  printf("Usage: s21_grep [options] pattern [file...]\n");
  printf("Options:\n");
  // printf("  -e pattern              use pattern as the expression to
  // match\n");
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

int parse_flags(int argc, char *argv[], char **pattern) {
  *pattern = NULL;
  int for_return = 0;
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
      }
      // else if (argv[i][1] == 'e') {
      //     if (argv[i][2] != '\0') {
      //         *pattern = (argv[i] + 2);
      //         printf("%s", *pattern);
      //         flags |= FLAG_FROM_FILE;
      //     } else if (i + 1 < argc && argv[i + 1][0] != '-') {
      //         *pattern = (argv[++i]);
      //         printf("%s", *pattern);
      //         flags |= FLAG_EXTENDED_PATTERN;
      //     } else {
      //         printf("Option -e requires pattern argument.\n");
      //         display_usage();
      //         exit(EXIT_FAILURE);
      //     }
      // }
      else {
        int j = 1;
        while (argv[i][j] != '\0') {
          switch (argv[i][j]) {
            // case 'e':
            //     if (i + 1 < argc) {
            //         *pattern = argv[++i];
            //         flags |= FLAG_EXTENDED_PATTERN;
            //     } else {
            //         printf("Option -e requires an argument.\n");
            //         display_usage();
            //         exit(EXIT_FAILURE);
            //     }
            //     break;
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
              flags |= FLAG_NO_NAMES;  // FLAG_NO_NAMES NO MNE LEN
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
      // Pattern argument found, stop processing options
      *pattern = argv[i];
      // return i + 1;
      for_return = i;
    }
  }
  if (no_flag == 0) {
    *pattern = argv[1];
    return 2;
  }

  return for_return;  // No pattern found
}

void grep_file(char *filename, char *pattern, int file_count) {
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
  // printf("%s = re", re);
  // printf("%s = file", filename);
  while (fgets(line, sizeof(line), file) != NULL) {
    int match;
    int ovector[30];
    int rc = pcre_exec(re, NULL, line, strlen(line), 0, 0, ovector,
                       sizeof(ovector) / sizeof(int));
    // printf("%d = rc\n", rc);

    if (flags & FLAG_INVERT_MATCH) {
      match = (rc == PCRE_ERROR_NOMATCH);
    } else {
      match = (rc >= 0);
    }
    // printf("%d = match\n", match);
    if (match) {
    // printf("match count %d FILENAME = %s\n", match_count, filename);
      match_count++;
      if (flags & FLAG_COUNT_ONLY) {
        // printf("match count %d & break FILENAME = %s\n", match_count, filename);
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
    if (file_count > 1 && !(flags & FLAG_NO_NAMES)){
        printf("%s:", filename);
    }
    printf("%d\n", match_count);
  }

  fclose(file);
  pcre_free(re);
}

int main(int argc, char *argv[]) {
  {
    char *pattern;
    int file_arg_start = parse_flags(argc, argv, &pattern);
    // printf("%s posle pars\n", pattern);
    if (!pattern) {
      printf("Pattern not specified.\n");
      display_usage();
      return EXIT_FAILURE;
    }

    int flags_count = 0;
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
    // printf("%d = file_count\n", file_count);
    // printf("%d = file_arg_start\n", file_arg_start);
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
            // printf("%s на вход\n", argv[i]);
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

    // Display each specified file
    // printf("%d = file_arg_start\n", file_arg_start);
    // for (int i = file_arg_start; i < argc; i++) {
    //     // printf("%s на вход\n", argv[i]);
    //     grep_file(argv[i], pattern, file_count);
    // }
  }
  // printf("%d = file_arg_start\n", file_arg_start);

  return EXIT_SUCCESS;
}
