#ifndef S21_CAT_H
#define S21_CAT_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

typedef struct {
  int b;
  int n;
  int s;
  int v;
  int E;
  int T;
} flags;

extern struct option long_options[];

int print_file(char *filename, flags options, int *count_lines);
void non_print(char c);

#endif
