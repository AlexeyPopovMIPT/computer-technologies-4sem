#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct HashTable;
typedef struct HashTable HashTable;

#define HT_ALREADY_EXISTS 1
#define HT_OK 0
#define HT_MEMORY_ERROR (-1)

HashTable *ht_create (size_t size);

void ht_destroy (HashTable *ht);

int ht_insert (HashTable *ht, const char *value);

bool ht_contains (HashTable *ht, const char *value);

bool ht_erase (HashTable *ht, const char *value);

bool ht_testEmptiness (HashTable *ht);

bool ht_testEquality (HashTable *ht, const char **start, const char **end);

#endif // !__HASHTABLE_H__
