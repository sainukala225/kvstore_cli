#include "kvstore.h"
#include "helpers.h"
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*********************************************************************
 *                              macros                               *
 *********************************************************************/

#define BUCKET_SIZE 101

/*********************************************************************
 *                          custom types                             *
 *********************************************************************/

typedef enum valuetype { integer, string, Double } valuetype;

typedef struct node {
  char *key;
  union value {
    int int_value;
    char *str_value;
    double double_value;
  } value;
  valuetype type;
  struct node *next;
} node;

typedef struct kvstore_t {
  node *bucket[BUCKET_SIZE];
  int total_items;
  int occupied_buckets;
  int int_items;
  int double_items;
  int str_items;
} kvstore_t;

/*********************************************************************
 *                     forward declarations                          *
 *********************************************************************/

static int hash(const char *key);
static void *set_key_value(node *item, const char *value);
static void *get_key(Kvstore store, const char *key);

/*********************************************************************
 *                       kvstore Funtions                            *
 *********************************************************************/

Kvstore kvstore_create() {
  Kvstore store = calloc(1, sizeof(kvstore_t));
  return store;
}

void kvstore_free(Kvstore store) {
  for (int i = 0; i < BUCKET_SIZE; i++) {
    node *curr = store->bucket[i], *next;
    while (curr) {
      free(curr->key);
      if (curr->type == string) {
        free(curr->value.str_value);
      }
      next = curr->next;
      free(curr);
      curr = next;
    }
    store->bucket[i] = NULL;
  }
  free(store);
}

void kvstore_clear(Kvstore store) {
  for (int i = 0; i < BUCKET_SIZE; i++) {
    node *curr = store->bucket[i], *next;
    while (curr) {
      free(curr->key);
      if (curr->type == string) {
        free(curr->value.str_value);
      }
      next = curr->next;
      free(curr);
      curr = next;
    }
    store->bucket[i] = NULL;
  }

  // Reset all the statistics to 0
  store->total_items = 0;
  store->occupied_buckets = 0;
  store->int_items = 0;
  store->double_items = 0;
  store->str_items = 0;
}

void kvstore_stats(Kvstore store) {
  double load_factor = (double)store->total_items / BUCKET_SIZE;
  printf("********************* stats of kvstore ***********************\n");
  printf("* The bucket size (capacity)     : %3d                       *\n",
         BUCKET_SIZE);
  printf("* The number of occupied_buckets : %3d                       *\n",
         store->occupied_buckets);
  printf("* The total items in the store   : %3d                       *\n",
         store->total_items);
  printf("* The load factor                : %.2f                      *\n",
         load_factor);
  printf("* The number of int items        : %3d                       *\n",
         store->int_items);
  printf("* The number of double items     : %3d                       *\n",
         store->double_items);
  printf("* The number of string items     : %3d                       *\n",
         store->str_items);
  printf("**************************************************************\n");
}

/*********************************************************************
 *                         key functions                             *
 *********************************************************************/

bool put_key(Kvstore store, const char *key, const char *value) {

  if (store == NULL) {
    errorf("Error : kvstore can't be null\n");
    return false;
  }

  node *existing_item = get_key(store, key);
  if (existing_item != NULL) {

    valuetype old_type = existing_item->type;
    if (set_key_value(existing_item, value) == NULL) {
      return false;
    }
    if (old_type != existing_item->type) {
      switch (old_type) {
      case string:
        store->str_items--;
        break;
      case integer:
        store->int_items--;
        break;
      case Double:
        store->double_items--;
        break;
      }
      switch (existing_item->type) {
      case string:
        store->str_items++;
        break;
      case integer:
        store->int_items++;
        break;
      case Double:
        store->double_items++;
        break;
      }
    }
    return true;
  }

  node *new_item = calloc(1, sizeof(node));

  if (new_item == NULL) {
    return false;
  }

  char *new_item_key = malloc(strlen(key) + 1);
  if (new_item_key == NULL) {
    free(new_item);
    return false;
  }
  strcpy(new_item_key, key);
  new_item->key = new_item_key;

  if (set_key_value(new_item, value) == NULL) {
    free(new_item_key);
    free(new_item);
    return false;
  }

  store->total_items++;
  switch (new_item->type) {
  case string:
    store->str_items++;
    break;
  case integer:
    store->int_items++;
    break;
  case Double:
    store->double_items++;
    break;
  }

  int pos = hash(key);
  node *HEAD = store->bucket[pos];

  if (HEAD == NULL) {
    store->bucket[pos] = new_item;
    store->occupied_buckets++;
  }

  else {
    while (HEAD->next != NULL) {
      HEAD = HEAD->next;
    }
    HEAD->next = new_item;
  }
  return true;
}

static void *get_key(Kvstore store, const char *key) {
  if (store == NULL || key == NULL) {
    return NULL;
  }

  int pos = hash(key);

  node *curr = store->bucket[pos];
  while (curr != NULL) {
    if (!strcmp(curr->key, key)) {
      return curr;
    }
    curr = curr->next;
  }
  return curr;
}

void print_key(Kvstore store, const char *key) {
  if (store == NULL || key == NULL) {
    printf("please enter proper values\n");
    return;
  }
  node *item = get_key(store, key);
  if (!item) {
    printf("Item doesn't exist\n");
    return;
  }
  switch (item->type) {
  case integer:
    printf("%s : %d\n", key, item->value.int_value);
    break;
  case string:
    printf("%s : %s\n", key, item->value.str_value);
    break;
  case Double:
    printf("%s : %lf\n", key, item->value.double_value);
    break;
  }
}

int delete_key(Kvstore store, const char *key) {
  if (store == NULL || key == NULL) {
    return 0;
  }

  int pos = hash(key);

  node *curr = store->bucket[pos], *prev = NULL;
  while (curr != NULL) {
    if (!strcmp(curr->key, key)) {
      break;
    }
    prev = curr;
    curr = curr->next;
  }

  if (curr == NULL) {
    return 0;
  }

  if (prev != NULL) {
    prev->next = curr->next;
  } else {
    store->bucket[pos] = curr->next;
  }

  store->total_items--;
  switch (curr->type) {
  case string:
    store->str_items--;
    break;
  case integer:
    store->int_items--;
    break;
  case Double:
    store->double_items--;
    break;
  }

  if (store->bucket[pos] == NULL) {
    store->occupied_buckets--;
  }
  free(curr->key);
  if (curr->type == string) {
    free(curr->value.str_value);
  }
  free(curr);

  return 1;
}

/*********************************************************************
 *                      File save/load Funtions                      *
 *********************************************************************/

int save_to_file(Kvstore store, char *filepath) {
  CLEANUP(close_file) FILE *fileptr = fopen(filepath, "w");
  if (!fileptr) {
    errorf("Error: Failed to create %s to save store\n", filepath);
    return 1;
  }
  int status;
  int dftries;

  for (int i = 0; i < BUCKET_SIZE; i++) {
    node *curr = store->bucket[i];
    while (curr != NULL) {
      switch (curr->type) {
      case integer:
        status = fprintf(fileptr, "PUT '%s' '%d'\n", curr->key,
                         curr->value.int_value);
        break;
      case Double:
        status = fprintf(fileptr, "PUT '%s' '%lf'\n", curr->key,
                         curr->value.double_value);
        break;
      case string:
        status = fprintf(fileptr, "PUT '%s' '%s'\n", curr->key,
                         curr->value.str_value);
        break;
      }
      if (status < 0) {
        errorf("Error: Failed when writing to file\n ");
        for (dftries = 1; dftries <= 10; dftries++) {
          if (!remove(filepath)) {
            return 1;
          }
        }
        if (dftries > 10) {
          errorf("Error: Failed to delete the file %s", filepath);
          return 1;
        }
      }
      curr = curr->next;
    }
  }
  return 0;
}

int load_from_file(Kvstore store, char *filepath) {
  CLEANUP(close_file) FILE *fileptr = fopen(filepath, "r");
  if (!fileptr) {
    errorf("Error: Failed to open the file %s\n", filepath);
    return 1;
  }
  char cmd[MAX_WORD_SIZE + 1];
  char arg1[MAX_WORD_SIZE + 1];
  char arg2[MAX_WORD_SIZE + 1];

  int line_count = 0;
  while (true) {
    line_count++;
    read_line_status status = read_line(fileptr);

    switch (status) {
    case REACHED_EOF:
      return 0;
    case READ_LINE_SUCCESS:
      break;
    case LINE_LIMIT_EXCEEDED:
      errorf("Error : The command size on line %d should be under %d\n",
             line_count, MAX_LINE_SIZE);
      continue;
    }

    // read the command and ignore it
    read_word(cmd);
    read_word(arg1); // get the arg1
    read_word(arg2); // get the arg2
    put_key(store, arg1, arg2);
  }
  return 0;
}
/*********************************************************************
 *                      Helper Funtions                              *
 *********************************************************************/

static void *set_key_value(node *item, const char *value) {

  char *oldstrvalue = item->type == string ? item->value.str_value : NULL;

  char *end;
  errno = 0;
  intmax_t val = strtoimax(value, &end, 0);
  if (end != value && *end == '\0' && errno != ERANGE) {
    item->type = integer;
    if (val > INT_MAX) {
      errorf("The value %s is higher than max int value (%d) so capping at "
             "%d\n",
             value, INT_MAX, INT_MAX);
      item->value.int_value = INT_MAX;
    } else if (val < INT_MIN) {
      errorf("The value %s is less than min int value (%d) so capping at "
             "%d\n",
             value, INT_MIN, INT_MIN);
      item->value.int_value = INT_MIN;
    } else {
      item->value.int_value = (int)val;
    }
  } else {
    errno = 0;
    double d = strtod(value, &end);
    if (end != value && *end == '\0' && errno != ERANGE) {
      item->type = Double;
      item->value.double_value = d;
    } else {
      char *item_strval = malloc(strlen(value) + 1);
      if (item_strval == NULL) {
        return NULL;
      }

      strcpy(item_strval, value);
      item->value.str_value = item_strval;
      item->type = string;
    }
  }
  free(oldstrvalue);
  return item;
}

// djb2 hashing algorithm
static int hash(const char *key) {
  unsigned long hash_value = 5381;
  for (const char *c = key; *c != '\0'; c++) {
    hash_value = ((hash_value << 5) + hash_value) + ((unsigned char)*c);
  }
  return hash_value % BUCKET_SIZE;
}
