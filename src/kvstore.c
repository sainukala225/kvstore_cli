#include "kvstore.h"
#include <ctype.h>
#include <errno.h>
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
static bool is_valid_number(const char *str);
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

void kvstore_stats(Kvstore store) {
  printf("********************* stats of kvstore ***********************\n");
  printf("* The number of occupied_buckets : %3d                       *\n",
         store->occupied_buckets);
  printf("* The total items in the store   : %3d                       *\n",
         store->total_items);
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
    printf("Item doesn't exist");
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
 *                      Helper Funtions                              *
 *********************************************************************/

static void *set_key_value(node *item, const char *value) {

  char *oldstrvalue = item->type == string ? item->value.str_value : NULL;

  valuetype item_valtype = string;

  if (!is_valid_number(value)) {

    char *item_strval = malloc(strlen(value) + 1);
    if (item_strval == NULL) {
      return NULL;
    }

    strcpy(item_strval, value);
    item->value.str_value = item_strval;
    item->type = item_valtype;
  } else {
    bool is_float = false;
    for (int i = 0; value[i] != '\0'; i++) {
      if (value[i] == '.' || value[i] == 'e' || value[i] == 'E') {
        is_float = true;
        break;
      }
    }

    if (is_float) {
      item->value.double_value = atof(value);
      item_valtype = Double;
    } else {
      item->value.int_value = strtol(value, NULL, 10);
      item_valtype = integer;
    }

    item->type = item_valtype;
  }

  free(oldstrvalue);
  return item;
}

// djb2 hashing algorithm
static int hash(const char *key) {
  unsigned long hash_value = 5381;
  for (const char *c = key; *c != '\0'; c++) {
    hash_value = ((hash_value << 5) + hash_value) + *c;
  }
  return hash_value % BUCKET_SIZE;
}

static bool is_valid_number(const char *str) {
  if (str == NULL || *str == '\0') {
    return false;
  }

  char *endptr;
  errno = 0;
  strtod(str, &endptr);

  if (*endptr != '\0') {
    return false;
  }

  if (endptr == str) {
    return false;
  }

  if (errno == ERANGE) {
    return false;
  }
  bool has_digit = false;
  for (const char *p = str; *p != '\0'; p++) {
    if (isdigit((unsigned char)*p)) {
      has_digit = true;
      break;
    }
  }
  if (!has_digit) {
    return false;
  }

  return true;
}
