#ifndef HELPER_H
#define HELPER_H
/*********************************************************************
 *                             macros                                *
 *********************************************************************/

#include <stdio.h>
#define MAX_WORD_SIZE 100
#define MAX_LINE_SIZE 1000

/*********************************************************************
 *                             enums                                 *
 *********************************************************************/

typedef enum {
  READ_LINE_SUCCESS,
  REACHED_EOF,
  LINE_LIMIT_EXCEEDED
} read_line_status;

void errorf(const char *format, ...);
int read_line(FILE *stream);
void read_word(char word[]);
#endif
