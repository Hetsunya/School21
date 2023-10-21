#ifndef CAT_H
#define CAT_H

#define FLAG_NUMBER_NONBLANK 1
#define FLAG_DISPLAY_EOL 2
#define FLAG_NUMBER_LINES 4
#define FLAG_SQUEEZE_BLANK 8
#define FLAG_DISPLAY_TABS 16

extern int parse_flags(int argc, char *argv[]);
extern void display_file(char *filename, int flags);

#endif /* CAT_H */
