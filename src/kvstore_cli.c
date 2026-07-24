#include "../include/kvstore.h"
#include <stdlib.h>
int main() {
  Kvstore store = kvstore_create();

  put_key(store, "name", "Alice");
  put_key(store, "age", "25");
  put_key(store, "pi", "3.14159");

  put_key(store, "name", "30");

  // delete_key(store, "pi");
  delete_key(store, "non_existent_key");

  print_key(store, "age");

  kvstore_stats(store);
  kvstore_free(store);

  return EXIT_SUCCESS;
}
