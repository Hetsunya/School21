#include "s21_cat.h"

struct option long_options[] = {
    {"number-nonblank", no_argument, 0, 'b'},
    {"number", no_argument, 0, 'n'},
    {"squeeze-blank", no_argument, 0, 's'},
    {0, 0, 0, 0}};


int main(int argc, char *argv[]) {
  char get_opt;
  int error = 0, op_index = 0;
  int count_lines = 0;
  flags options = {0, 0, 0, 0, 0, 0};

  while (!error && (get_opt = getopt_long(argc, argv, ":benstvET", long_options,
                                          &op_index)) != -1) {
    switch (get_opt) {
        case 'b':
          options.b = 1;
          options.n = 0;
          break;
        case 'e':
          options.E = 1;
          options.v = 1;
          break;
        case 't':
          options.T = 1;
          options.v = 1;
          break;
        case 'n':
          options.n = !options.b;
          break;
        case 's':
          options.s = 1;
          break;
        case 'v':
          options.v = 1;
          break;
        case 'E':
          options.E = 1;
          break;
        case 'T':
          options.T = 1;
          break;
        default:
          error = 1;
          break;
      }
    }

  if (!error) {
    while (optind < argc) {
      if (print_file(argv[optind], options, &count_lines))
        printf("%s: No such file or directory\n", argv[optind]);
      optind++;
    }
  } else {
    printf("Error command line arguments!\n");
  }

  return 0;
}

int print_file(char *filename, flags options, int *count_lines) {
  char ch = '\0';
  int result;
  int nlc = 1;  // new lines count
  FILE *f = fopen(filename, "r");

  !f ? (result = 0) : (result = 1);
  
  while (result && (ch = fgetc(f)) != EOF) {
      if (ch == '\n' && options.s && nlc > 1) {
        ch = fgetc(f);
        // if(ch != '\n') {
        //   printf("%c", ch);
        // } РАБОТАЕТ НО ЛОМАЕТ В ДРУГИХ МЕСТАХ XD );
        continue;
      }

      if (nlc && (options.n || (ch != '\n' && options.b)))
        printf("%6d\t", ++(*count_lines));

      if (options.E && ch == '\n') printf("$");

      if (options.T && ch == '\t')
        printf("^I");
      else {
        ch == '\n' ? nlc++ : (nlc = 0);
        options.v ? non_print(ch) : printf("%c", ch);
      }
    }


  if (result) fclose(f);

  return !result;
}

  void non_print(char c) {
    if (c < -96)
      printf("M-^%c", c + 192);
    else if (c < 0)
      printf("M-%c", c + 128);
    else if (c == 9 || c == 10)
      printf("%c", c);
    else if (c < 32)
      printf("^%c", c + 64);
    else if (c < 127)
      printf("%c", c);
    else
      printf("^?");
  }