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
  LINE_LIMIT_EXCEEDED,
  READ_LINE_FAILED,
} read_line_status;

typedef enum {
  WORD_OK,
  WORD_INVALID_ESCAPE,
  WORD_TOO_LONG,
  WORD_UNTERMINATED_QUOTE,
} read_word_status;

[[gnu::format(printf, 1, 2)]]
void errorf(const char *format, ...);
void close_file(FILE **fp);
void free_mem(void *ptr);
read_line_status read_line(FILE *stream);
read_word_status read_word(char word[]);
bool handle_read_word_status(read_word_status status, const char *wordtype,
                             const char *filepath, int line_number);
void conv_str_to_lowcase(char string[]);
#endif
