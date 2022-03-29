#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct HashTable;
typedef struct HashTable HashTable;

typedef void (ht_iter_func_t) (const char *str, void *context);

#define HT_ALREADY_EXISTS 1
#define HT_OK 0
#define HT_MEMORY_ERROR (-1)

HashTable *ht_create (size_t size);

void ht_destroy (HashTable *ht);

int ht_insert (HashTable *ht, const char *value);

bool ht_contains (const HashTable *ht, const char *value);

bool ht_erase (HashTable *ht, const char *value);

bool ht_testEquality (HashTable *ht, const char **start, const char **end);

void ht_foreach (const HashTable *ht, ht_iter_func_t *func, void *context);

#endif // !__HASHTABLE_H__
