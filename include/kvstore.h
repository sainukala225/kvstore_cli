#ifndef KVSTORE_H
#define KVSTORE_H

typedef struct kvstore_t *Kvstore;

void *put_key(Kvstore store, char *key, char *value);
void *get_key(Kvstore store, char *key);
int delete_key(Kvstore store, char *key);
void stats(Kvstore store);
int save_to_file(Kvstore store, char *filepath);
int load_from_file(Kvstore store, char *filepath);
#endif
