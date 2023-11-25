CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11

all : s21_grep

s21_cat: s21_grep.c s21_grep.h
	$(CC) $(CFLAGS) -o s21_grep s21_grep.c

clean:
	rm -f s21_grep
