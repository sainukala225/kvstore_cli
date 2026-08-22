#include "helpers.h"
#include "kvstore.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*********************************************************************
 *                      forward declarations                         *
 *********************************************************************/

static void conv_str_to_lowcase(char string[]);
static void print_help_message();
static bool handle_read_word_status(read_word_status status,
                                    const char *wordtype);

/*********************************************************************
 *                            main program                           *
 *********************************************************************/

int main(int argc, char *argv[]) {

  bool test_mode = false;

  if (argc == 2 && !strcmp(argv[1], "-T")) {
    test_mode = true;
  } else if (argc > 2) {
    errorf("Error : Invalid program Initialization\n");
    return EXIT_FAILURE;
  }

  Kvstore store = kvstore_create();
  if (store == NULL) {
    errorf("failed to create the store, exiting program.\n");
    return EXIT_FAILURE;
  }

  if (!test_mode) {
    print_help_message();
  }

  char cmd[MAX_WORD_SIZE + 1];
  char arg1[MAX_WORD_SIZE + 1];
  char arg2[MAX_WORD_SIZE + 1];

  while (true) {

    if (!test_mode) {
      printf("enter the command : ");
    }

    // read the whole line
    read_line_status line_status = read_line(stdin);

    switch (line_status) {
    case REACHED_EOF:
      kvstore_free(store);
      if (!test_mode) {
        printf("reached EOF. exiting program\n");
      }
      return EXIT_SUCCESS;
    case READ_LINE_SUCCESS:
      break;
    case READ_LINE_FAILED:
      errorf("Error : Failed to read the line\n");
      errorf("Exiting the program\n");
      kvstore_free(store);
      exit(EXIT_FAILURE);
    case LINE_LIMIT_EXCEEDED:
      errorf("Error : The command size should be under %d.\n", MAX_LINE_SIZE);
      continue;
    }

    if (!handle_read_word_status(read_word(cmd), "Command")) {
      continue;
    }

    // read just the command and convert it to lower case
    conv_str_to_lowcase(cmd);

    if (!handle_read_word_status(read_word(arg1), "Key")) { // get the arg1
      continue;
    }

    if (!handle_read_word_status(read_word(arg2), "Value")) { // get the arg2
      continue;
    }

    // commands with no arguments
    if (!strcmp(cmd, "exit")) {
      if (arg1[0]) {
        errorf("Error: 'exit' takes no arguments. Type 'help' for usage.\n");
      } else {
        break;
      }
    } else if (!strcmp(cmd, "help")) {
      if (arg1[0]) {
        errorf("Error: 'help' takes no arguments. Type 'help' for usage.\n");
      } else {
        print_help_message();
      }
    } else if (!strcmp(cmd, "free")) {
      if (arg1[0]) {
        errorf("Error: 'free' takes no arguments. Type 'help' for usage.\n");
      } else {
        kvstore_clear(store);
      }
    } else if (!strcmp(cmd, "stats")) {
      if (arg1[0]) {
        errorf("Error: 'stats' takes no arguments. Type 'help' for usage.\n");
      } else {
        kvstore_stats(store);
      }
    }

    // commands with just one argument
    else if (!strcmp(cmd, "delete")) {
      if (!arg1[0]) { // key
        errorf(
            "Error: 'delete' requires key argument. Type 'help' for usage.\n");
      } else if (arg2[0]) {
        errorf("Error: 'delete' takes only one argument. Type 'help' for "
               "usage.\n");
      } else {
        delete_key(store, arg1);
      }
    } else if (!strcmp(cmd, "get")) {
      if (!arg1[0]) {
        errorf("Error: 'get' requires key argument. Type 'help' for usage.\n");
      } else if (arg2[0]) {
        errorf(
            "Error: 'get' takes only one argument. Type 'help' for usage.\n");
      } else {
        print_key(store, arg1);
      }
    }

    else if (!strcmp(cmd, "save")) {
      if (!arg1[0]) {
        errorf("Error: 'save' requires filepath argument. Type 'help' for "
               "usage.\n");
      } else if (arg2[0]) {
        errorf(
            "Error: 'save' takes only one argument. Type 'help' for usage.\n");
      } else {
        if (!save_to_file(store, arg1)) {
          printf("store saved to file %s successfully\n", arg1);
        }
      }
    }

    else if (!strcmp(cmd, "load")) {
      if (!arg1[0]) {
        errorf("Error: 'load' requires filepath argument. Type 'help' for "
               "usage.\n");
      } else if (arg2[0]) {
        errorf(
            "Error: 'load' takes only one argument. Type 'help' for usage.\n");
      } else {
        if (!load_from_file(store, arg1)) {
          printf("store loaded from file %s successfully\n", arg1);
        } else {
          errorf("Error : Failed to load the store\n");
        }
      }
    }

    // command with two arguments
    else if (!strcmp(cmd, "put")) {
      char extra_arg[MAX_WORD_SIZE + 1];
      read_word(extra_arg);
      if (!arg1[0]) { // key
        errorf("Error: 'put' requires key argument. Type 'help' for usage.\n");
      } else if (!arg2[0]) { // value
        errorf(
            "Error: 'put' requires value argument. Type 'help' for usage.\n");
      } else if (extra_arg[0]) {
        errorf(
            "Error: 'put' takes only two argument. Type 'help' for usage.\n");
      } else {
        put_key(store, arg1, arg2);
      }
    } else {
      if (!cmd[0]) {
        continue; // To skip empty lines in test mode
      } else {
        errorf("Error: Invalid command. Type 'help' for a list of commands.\n");
      }
    }
  }

  kvstore_free(store);
  if (!test_mode) {
    printf("exited program successfully.\n");
  }
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
  printf("* SAVE FILEPTH      - saves the store to the file                    "
         "  *\n");
  printf("* LOAD FILEPTH      - loads the store from the file                  "
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
 *                      Helper Functions                              *
 *********************************************************************/

static void conv_str_to_lowcase(char string[]) {
  for (unsigned int i = 0, len = strlen(string); i < len; i++) {
    string[i] = tolower((unsigned char)string[i]);
  }
}

static bool handle_read_word_status(read_word_status status,
                                    const char *wordtype) {
  bool success = false;
  switch (status) {
  case WORD_OK:
    success = true;
    break;
  case WORD_TOO_LONG:
    errorf("Error : %s is too long (should be under %d)\n", wordtype,
           MAX_WORD_SIZE);
    break;
  case WORD_INVALID_ESCAPE:
    errorf("Error : Invalid escape in the %s\n", wordtype);
    break;
  case WORD_UNTERMINATED_QUOTE:
    errorf("Error : Unterminated quote in the %s\n", wordtype);
    break;
  }
  return success;
}
