#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>

/*********************************************************************
 *                             macros                                *
 *********************************************************************/

#define MAX_WORD_SIZE 100
#define MAX_LINE_SIZE 1000
#define CLEANUP(fn) __attribute__((cleanup(fn)))
/*********************************************************************
 *                             enums                                 *
 *********************************************************************/

typedef enum {
  READ_LINE_SUCCESS,
  REACHED_EOF,
  LINE_LIMIT_EXCEEDED
} read_line_status;

[[gnu::format(printf, 1, 2)]]
void errorf(const char *format, ...);
void close_file(FILE **fp);
int read_line(FILE *stream);
void read_word(char word[]);
#endif
