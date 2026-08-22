#include "helpers.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
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
void close_file(FILE **fp) {
  if (*fp) {
    fclose(*fp);
  }
}

void free_mem(void *ptr) { free(*(void **)ptr); }

read_line_status read_line(FILE *stream) {
  if (fgets(line, sizeof line, stream) == NULL) {
    if (feof(stream)) {
      return REACHED_EOF;
    } else {
      return READ_LINE_FAILED;
    }
  }

  if (strchr(line, '\n') == NULL && !feof(stream)) {
    // Buffer filled before the line ended, so the rest is still queued.
    //   Drain it, or it becomes a bogus next command.
    int ch;
    while ((ch = fgetc(stream)) != '\n' && ch != EOF)
      ;
    return LINE_LIMIT_EXCEEDED;
  }

  line[strcspn(line, "\n")] = '\0';
  CCHAR_POS_ON_LINE = 0;
  return READ_LINE_SUCCESS;
}

// On any status other than WORD_OK the caller must abandon the line;
// the read position is left unspecified.
read_word_status read_word(char word[]) {

  if (line[CCHAR_POS_ON_LINE] == '\0') {
    word[0] = '\0';
    return WORD_OK;
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
    return WORD_OK;
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
    if (ch == '\\') {
      char next_char = line[ccounter_in_line + 1];
      if (next_char == word_end || next_char == '\\') {
        word[ccounter_in_word] = next_char;
        ccounter_in_line += 2;
        ccounter_in_word++;
        continue;
      } else {
        word[ccounter_in_word] = '\0';
        return WORD_INVALID_ESCAPE;
      }
    }

    ccounter_in_line++;
    word[ccounter_in_word] = ch;
    ccounter_in_word++;
  }
  word[ccounter_in_word] = '\0';

  char terminator = line[ccounter_in_line];
  if (ccounter_in_word == MAX_WORD_SIZE && terminator != word_end) {
    return WORD_TOO_LONG;
  } else if (ccounter_in_word < MAX_WORD_SIZE &&
             (word_end == 34 || word_end == 39) && terminator != word_end) {
    return WORD_UNTERMINATED_QUOTE;
  }
  // consume the delimiter
  if (terminator == word_end) {
    ccounter_in_line++;
  }

  CCHAR_POS_ON_LINE = ccounter_in_line;
  return WORD_OK;
}
