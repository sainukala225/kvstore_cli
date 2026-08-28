#ifndef KVSTORE_H
#define KVSTORE_H

typedef struct kvstore_t *Kvstore;
typedef struct kvstore_stats {
  int bucket_size;
  int occupied_buckets;
  int total_items;
  int int_items;
  int double_items;
  int str_items;
} Kvstore_stats;

Kvstore kvstore_create();
void kvstore_free(Kvstore store);
void kvstore_clear(Kvstore store);
bool put_key(Kvstore store, const char *key, const char *value);
void print_key(Kvstore store, const char *key);
bool delete_key(Kvstore store, const char *key);
Kvstore_stats kvstore_get_stats(Kvstore store);
void kvstore_stats(Kvstore store);
int save_to_file(Kvstore store, const char *filepath);
int load_from_file(Kvstore store, const char *filepath);
#endif
