#include "kvstore.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*********************************************************************
 *                             macros                                *
 *********************************************************************/

#define MAX_WORD_SIZE 100
#define MAX_LINE_SIZE 1000

/*********************************************************************
 *                    file scoped variables                          *
 *********************************************************************/

static char line[MAX_LINE_SIZE + 1];
static int CCHAR_POS_ON_LINE;

/*********************************************************************
 *                      forward declarations                         *
 *********************************************************************/

static int read_line();
static void read_word(char word[]);
static void conv_str_to_lowcase(char string[]);
static void print_help_message();

/*********************************************************************
 *                            main program                           *
 *********************************************************************/

int main() {

  Kvstore store = kvstore_create();
  if (store == NULL) {
    printf("failed to create the store, exiting program.\n");
    return EXIT_FAILURE;
  }

  print_help_message();
  char cmd[MAX_WORD_SIZE + 1];
  char key[MAX_WORD_SIZE + 1];
  char value[MAX_WORD_SIZE + 1];

  while (true) {

    printf("enter the command : ");

    // read the whole line
    if (!read_line()) {
      printf("reached EOF. exiting program\n");
      return EXIT_SUCCESS;
    }

    // read just the command and convert it to lower case
    read_word(cmd);
    conv_str_to_lowcase(cmd);

    read_word(key);   // get the key
    read_word(value); // get the value

    // commands with no arguments
    if (!strcmp(cmd, "exit")) {
      if (key[0]) {
        printf("Error: 'exit' takes no arguments. Type 'help' for usage.\n");
      } else {
        break;
      }
    } else if (!strcmp(cmd, "help")) {
      if (key[0]) {
        printf("Error: 'help' takes no arguments. Type 'help' for usage.\n");
      } else {
        print_help_message();
      }
    } else if (!strcmp(cmd, "free")) {
      if (key[0]) {
        printf("Error: 'free' takes no arguments. Type 'help' for usage.\n");
      } else {
        kvstore_clear(store);
      }
    } else if (!strcmp(cmd, "stats")) {
      if (key[0]) {
        printf("Error: 'stats' takes no arguments. Type 'help' for usage.\n");
      } else {
        kvstore_stats(store);
      }
    }

    // commands with just one argument
    else if (!strcmp(cmd, "delete")) {
      if (!key[0]) {
        printf(
            "Error: 'delete' requires key argument. Type 'help' for usage.\n");
      } else if (value[0]) {
        printf("Error: 'delete' takes only one argument. Type 'help' for "
               "usage.\n");
      } else {
        delete_key(store, key);
      }
    } else if (!strcmp(cmd, "get")) {
      if (!key[0]) {
        printf("Error: 'get' requires key argument. Type 'help' for usage.\n");
      } else if (value[0]) {
        printf(
            "Error: 'get' takes only one argument. Type 'help' for usage.\n");
      } else {
        print_key(store, key);
      }
    }

    // command with two arguments
    else if (!strcmp(cmd, "put")) {
      char extra_arg[MAX_WORD_SIZE + 1];
      read_word(extra_arg);
      if (!key[0]) {
        printf("Error: 'put' requires key argument. Type 'help' for usage.\n");
      } else if (!value[0]) {
        printf(
            "Error: 'put' requires value argument. Type 'help' for usage.\n");
      } else if (extra_arg[0]) {
        printf(
            "Error: 'put' takes only two argument. Type 'help' for usage.\n");
      } else {
        put_key(store, key, value);
      }
    } else {
      printf("Error: Invalid command. Type 'help' for a list of commands.\n");
    }
  }

  kvstore_free(store);
  printf("exitted program successfully.\n");
  return EXIT_SUCCESS;
}

/*********************************************************************
 *                          Help message                             *
 *********************************************************************/

static void print_help_message() {
  printf("******************* This is a cli tool for a kvstore "
         "*******************\n");
  printf("****************      list of all available tools       "
         "****************\n");
  printf("* GET KEY           - gets the value for the key and prints it       "
         "  *\n");
  printf("* PUT KEY VALUE     - stores the key value pair in the store         "
         "  *\n");
  printf("* DELETE KEY        - deletes the key from the store                 "
         "  *\n");
  printf("* stats             - displays the stats of the store                "
         "  *\n");
  printf("* free              - deletes all the keys from the store            "
         "  *\n");
  printf("* exit              - quit the program                               "
         "  *\n");
  printf("* help              - displays this message again                    "
         "  *\n");
  printf("*********************************************************************"
         "***\n");
  printf("* Note: Commands are case-insensitive.                               "
         "  *\n");
  printf("* Note: Keys and values with spaces can be wrapped in 'single "
         "quotes'  *\n");
  printf("*********************************************************************"
         "***\n");
}

/*********************************************************************
 *                      Helper Funtions                              *
 *********************************************************************/

static int read_line() {
  int ch; // have char as int so comparison with EOF works
  int i = 0;
  CCHAR_POS_ON_LINE = 0;
  while (i < MAX_LINE_SIZE && (ch = getchar()) != '\n' && ch != EOF) {
    line[i++] = ch;
  }
  line[i] = '\0';
  return ch == EOF ? 0 : 1;
}

static void read_word(char word[]) {

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

static void conv_str_to_lowcase(char string[]) {
  for (unsigned int i = 0, len = strlen(string); i < len; i++) {
    string[i] = tolower((unsigned char)string[i]);
  }
}
