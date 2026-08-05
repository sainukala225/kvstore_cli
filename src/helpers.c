#include "helpers.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/*********************************************************************
 *                    file scoped variables                          *
 *********************************************************************/

char line[MAX_LINE_SIZE + 1];
int CCHAR_POS_ON_LINE;

void errorf(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);
}

int read_line(FILE *stream) {
  if (fgets(line, sizeof line, stream) == NULL)
    return REACHED_EOF;

  if (strchr(line, '\n') == NULL && !feof(stream)) {
    // Buffer filled before the line ended, so the rest is still queued.
    //   Drain it, or it becomes a bogus next command.
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
      ;
    return LINE_LIMIT_EXCEEDED;
  }

  line[strcspn(line, "\n")] = '\0';
  CCHAR_POS_ON_LINE = 0;
  return READ_LINE_SUCCESS;
}

void read_word(char word[]) {

  if (line[CCHAR_POS_ON_LINE] == '\0') {
    word[0] = '\0';
    return;
  }

  char ch;
  int ccounter_in_line = CCHAR_POS_ON_LINE;

  while (true) {
    ch = line[ccounter_in_line];
    if (ch != ' ') {
      break;
    }
    ccounter_in_line++;
  }

  if (ch == '\0') {
    word[0] = '\0';
    return;
  }

  char word_end;

  int ccounter_in_word = 0;
  // test the last read char
  switch (ch) {
  case 34: // char "
    word_end = 34;
    break;
  case 39: // char '
    word_end = 39;
    break;
  default:
    word_end = 32; // space
    word[ccounter_in_word] = ch;
    ccounter_in_word++;
  }

  ccounter_in_line++;

  while (ccounter_in_word < MAX_WORD_SIZE &&
         (ch = line[ccounter_in_line]) != word_end && ch != '\0') {
    ccounter_in_line++;
    word[ccounter_in_word] = ch;
    ccounter_in_word++;
  }
  word[ccounter_in_word] = '\0';

  // consume the delimiter
  if (ch == word_end) {
    ccounter_in_line++;
  }

  CCHAR_POS_ON_LINE = ccounter_in_line;
}
