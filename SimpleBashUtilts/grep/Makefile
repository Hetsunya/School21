CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c99
LDFLAGS = -lpcre

all : s21_grep

s21_grep: grep.c grep.h
	$(CC) $(CFLAGS) -o s21_grep grep.c $(LDFLAGS)

clean:
	rm -f s21_grep
	