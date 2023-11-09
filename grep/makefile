CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lpcre

s21_grep: grep.c grep.h
	$(CC) $(CFLAGS) -o s21_grep grep.c $(LDFLAGS)

clean:
	rm -f s21_grep
