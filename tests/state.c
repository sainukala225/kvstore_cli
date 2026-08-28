#include "helpers.h"
#include "kvstore.h"
#include <stdio.h>
#include <stdlib.h>

static int checks_passed = 0;

static void cleanup_store(Kvstore *store_ptr) { kvstore_free(*store_ptr); }

static bool totals_add_up(Kvstore_stats stats);
static bool type_counts_match(Kvstore_stats stats, int int_items,
                              int double_items, int str_items);
static bool check_state(Kvstore_stats stats, const char *position,
                        int int_items, int double_items, int str_items);

int main(void) {

  [[gnu::cleanup(cleanup_store)]]
  Kvstore store = kvstore_create();
  if (!store) {
    errorf("Error in state.c : Failed to create the store\n");
    return EXIT_FAILURE;
  }

  Kvstore_stats actual_stats;

  if (!put_key(store, "key", "abc")) {
    errorf("Error in state.c : Failed to put 'key' in store\n");
    return EXIT_FAILURE;
  }
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after put key abc", 0, 0, 1)) {
    return EXIT_FAILURE;
  }

  if (!put_key(store, "key", "123")) {
    errorf("Error in state.c : Failed to put 'key' in store\n");
    return EXIT_FAILURE;
  }
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after put key 123", 1, 0, 0)) {
    return EXIT_FAILURE;
  }

  if (!put_key(store, "key", "345")) {
    errorf("Error in state.c : Failed to put 'key' in store\n");
    return EXIT_FAILURE;
  }
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after put key 345", 1, 0, 0)) {
    return EXIT_FAILURE;
  }

  if (!delete_key(store, "key")) {
    errorf("Error in state.c : 'key' doesn't exist in the store\n");
    return EXIT_FAILURE;
  }
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after delete key", 0, 0, 0)) {
    return EXIT_FAILURE;
  }

  if (!put_key(store, "integer", "2")) {
    errorf("Error in state.c : Failed to put 'integer' in store\n");
    return EXIT_FAILURE;
  }
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after put integer 2", 1, 0, 0)) {
    return EXIT_FAILURE;
  }

  if (!put_key(store, "float", "4.11")) {
    errorf("Error in state.c : Failed to put 'float' in store\n");
    return EXIT_FAILURE;
  }
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after put float 4.11", 1, 1, 0)) {
    return EXIT_FAILURE;
  }

  if (!put_key(store, "string", "hello")) {
    errorf("Error in state.c : Failed to put 'string' in store\n");
    return EXIT_FAILURE;
  }
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after put string hello", 1, 1, 1)) {
    return EXIT_FAILURE;
  }

  kvstore_clear(store);
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after clear", 0, 0, 0)) {
    return EXIT_FAILURE;
  }

  // clearing an already empty store must be a no-op, not a double free
  kvstore_clear(store);
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after clearing an empty store", 0, 0, 0)) {
    return EXIT_FAILURE;
  }

  // the store must still be usable after being cleared
  if (!put_key(store, "a", "1")) {
    errorf("Error in state.c : Failed to put 'a' in store\n");
    return EXIT_FAILURE;
  }
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after put a 1", 1, 0, 0)) {
    return EXIT_FAILURE;
  }

  if (!put_key(store, "bx", "2")) {
    errorf("Error in state.c : Failed to put 'bx' in store\n");
    return EXIT_FAILURE;
  }
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after put bx 2", 2, 0, 0)) {
    return EXIT_FAILURE;
  }

  // deleting a key that was never inserted must fail and change nothing
  if (delete_key(store, "key_not_in_store")) {
    errorf("Error in state.c : delete_key reported success for a key that was "
           "never inserted\n");
    return EXIT_FAILURE;
  }
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after failed delete", 2, 0, 0)) {
    return EXIT_FAILURE;
  }

  // deleting one of two keys, so a chain has to survive the removal
  if (!delete_key(store, "bx")) {
    errorf("Error in state.c : 'bx' doesn't exist in the store\n");
    return EXIT_FAILURE;
  }
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after delete bx", 1, 0, 0)) {
    return EXIT_FAILURE;
  }

  kvstore_clear(store);
  actual_stats = kvstore_get_stats(store);
  if (!check_state(actual_stats, "after final clear", 0, 0, 0)) {
    return EXIT_FAILURE;
  }

  printf("state.c : %d checks passed\n", checks_passed);
  return EXIT_SUCCESS;
}

static bool totals_add_up(Kvstore_stats stats) {
  return stats.total_items ==
         (stats.int_items + stats.double_items + stats.str_items);
}

static bool type_counts_match(Kvstore_stats stats, int int_items,
                              int double_items, int str_items) {
  return stats.int_items == int_items && stats.double_items == double_items &&
         stats.str_items == str_items;
}

static bool check_state(Kvstore_stats stats, const char *position,
                        int int_items, int double_items, int str_items) {
  bool is_ok = true;

  if (!type_counts_match(stats, int_items, double_items, str_items)) {
    errorf("Error in state.c %s : Type counts don't match expected value\n",
           position);
    errorf("Expected : integers : %d, doubles : %d, strings : %d\n", int_items,
           double_items, str_items);
    errorf("got : integers : %d, doubles : %d, strings : %d\n", stats.int_items,
           stats.double_items, stats.str_items);
    is_ok = false;
  }

  if (!totals_add_up(stats)) {
    errorf("Error in state.c %s : Totals don't add up\n", position);
    errorf("integers : %d, doubles : %d, strings : %d, total_items : %d\n",
           stats.int_items, stats.double_items, stats.str_items,
           stats.total_items);
    is_ok = false;
  }

  if (stats.total_items == 0 && stats.occupied_buckets != 0) {
    errorf("Error in state.c %s : occupied buckets is non zero when total "
           "items are zero\n",
           position);
    is_ok = false;
  }

  if (stats.total_items > 0 && stats.occupied_buckets == 0) {
    errorf("Error in state.c %s : occupied buckets is zero when total items "
           "are non zero\n",
           position);
    is_ok = false;
  }

  if (stats.occupied_buckets > stats.total_items) {
    errorf(
        "Error in state.c %s : occupied buckets is greater than total items\n",
        position);
    is_ok = false;
  }

  if (stats.occupied_buckets > stats.bucket_size) {
    errorf("Error in state.c %s : occupied buckets is greater than bucket "
           "size\n",
           position);
    is_ok = false;
  }

  if (is_ok) {
    checks_passed++;
  }

  return is_ok;
}
