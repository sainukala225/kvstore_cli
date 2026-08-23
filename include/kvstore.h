#ifndef KVSTORE_H
#define KVSTORE_H

typedef struct kvstore_t *Kvstore;

Kvstore kvstore_create();
void kvstore_free(Kvstore store);
void kvstore_clear(Kvstore store);
bool put_key(Kvstore store, const char *key, const char *value);
void print_key(Kvstore store, const char *key);
bool delete_key(Kvstore store, const char *key);
void kvstore_stats(Kvstore store);
int save_to_file(Kvstore store, const char *filepath);
int load_from_file(Kvstore store, const char *filepath);
#endif
