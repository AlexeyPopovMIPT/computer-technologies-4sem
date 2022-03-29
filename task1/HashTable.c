#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "HashTable.h"

#ifdef FAULT_INJ
#include <time.h>
#define RANDOMLY_DIE (rand() % 16 == 0)
#else
#define RANDOMLY_DIE 0
#endif

size_t ht_calcHash (const char* str, size_t hashsize)
{
    size_t ret = 0;
    size_t a = 31415;
    size_t b = 27183;
    for (char c = *str;
         c; 
         c = *++str, a = a * b % (hashsize - 1))
    {
        ret = (a * ret + c) % hashsize;
    }

    return ret;
}


typedef struct TableEl
{
    char *data;
    struct TableEl *next;
} TableEl;


struct HashTable
{
    uint64_t hashsize;
    TableEl table [0];
};

// returns true iff a+b*c overflows
bool overflows (size_t a, size_t b, size_t c)
{
    size_t mul = a * b;
    if (mul == 0)
        return false;
    if (mul / a != b)
        return true;

    if ((ssize_t) mul + a < 0)
        return true;

    return false;
}

HashTable *ht_create (size_t size)
{
    if (overflows (sizeof (HashTable), size, sizeof (TableEl)))
        return NULL;

    HashTable *ht = (HashTable *) malloc (sizeof (HashTable) + size * sizeof (TableEl));
    if (ht == NULL || RANDOMLY_DIE)
    {
        free (ht);
        return NULL;
    }
    
    ht->hashsize = size;
    memset (ht->table, '\0', size * sizeof (TableEl));

    return ht;
}

void ht_destroy (HashTable *ht)
{
    if (ht == NULL) 
        return;
    for (int i = 0; i < ht->hashsize; ++i)
    {
        free (ht->table[i].data);
        TableEl *iter = ht->table[i].next;
        TableEl *tmp;
        while (iter != NULL)
        {
            free (iter->data);
            tmp = iter;
            iter = iter->next;
            free (tmp);
        }
    }
    free (ht);
}

// \returns pointer to TableEl, after which \value is encountered
// but if \value is in \start, returns NULL
// but if table doesn't contain \value, returns tail

// Assume TableEl *ret = ht_lookup (start, value);
// if ret == nullptr, then value is in start
// if ret->next is nullptr, then table doesn't contain value
// otherwise ret->next contains value
static TableEl *ht_lookup (const TableEl *start, const char *value)
{
    if (start->data == NULL)
        return (TableEl *)start;
    if (strcmp (start->data, value) == 0)
        return NULL;
    
    const TableEl *iter = start;
    for (; iter->next != NULL; iter = iter->next)
    {
        if (strcmp (iter->next->data, value) == 0)
            return (TableEl *)iter;
    }

    return (TableEl *)iter;
}

static int ht_insertAfter (TableEl *toInsert, const char *value)
{
    size_t len = strlen (value);

    toInsert->next = (TableEl *) malloc (sizeof (TableEl));

    if (toInsert->next == NULL || RANDOMLY_DIE)
    {
        free (toInsert->next);
        toInsert->next = NULL;
        return HT_MEMORY_ERROR;
    }

    toInsert->next->next = NULL;
    toInsert->next->data = (char *) malloc (len + 1);

    if (toInsert->next->data == NULL || RANDOMLY_DIE)
    {
        free (toInsert->next->data);
        free (toInsert->next);
        toInsert->next = NULL;
        return HT_MEMORY_ERROR;
    }

    memcpy (toInsert->next->data, value, len + 1);
    return HT_OK;
}

int ht_insert (HashTable *ht, const char *value)
{
    size_t hash = ht_calcHash (value, ht->hashsize);
    TableEl *toInsert = ht_lookup (&(ht->table[hash]), value);

    if (toInsert != NULL && toInsert->next == NULL)
    {
        if (toInsert->data == NULL)
        {
            size_t len = strlen (value);

            toInsert->data = (char *) malloc (len + 1);

            if (toInsert->data == NULL || RANDOMLY_DIE)
            {
                free (toInsert->data);
                toInsert->data = NULL;
                return HT_MEMORY_ERROR;
            }

            memcpy (toInsert->data, value, len + 1);
            return HT_OK;

        }
        else
            return ht_insertAfter (toInsert, value);
    }

    return HT_ALREADY_EXISTS;
}

bool ht_contains (const HashTable *ht, const char *value)
{
    size_t hash = ht_calcHash (value, ht->hashsize);
    TableEl *found = ht_lookup (&(ht->table[hash]), value);

    return found == NULL || found->next != NULL;
}

// returns true iff ht contained value
bool ht_erase (HashTable *ht, const char *value)
{
    size_t hash = ht_calcHash (value, ht->hashsize);
    TableEl *found = ht_lookup (&(ht->table[hash]), value);

    if (found == NULL)
    {
        free (ht->table[hash].data);
        if (ht->table[hash].next == NULL)
        {
            // value being erased was the only one with this hash, 
            // everything is simple
            ht->table[hash].data = NULL;
            return true;
        }
        else
        {
            // next element of singly-linked list becomes head, move to array
            TableEl *tmp = ht->table[hash].next;
            ht->table[hash] = *tmp;

            free (tmp);
            return true;
        }
        
    }

    if (found->next == NULL)
        return false;

    free (found->next->data);

    TableEl *tmp = found->next;
    found->next = tmp->next;

    free (tmp);
    return true;
}

void ht_foreach (const HashTable *ht, ht_iter_func_t *func, void *context)
{
    for (int i = 0; i < ht->hashsize; ++i)
    {
        if (ht->table[i].data != NULL)
            (*func) (ht->table[i].data, context);

        for (TableEl *iter = ht->table[i].next; iter != NULL; iter = iter->next)
            (*func) (iter->data, context);
    }
}










//////////////////////////////////////////
// TESTING FUNCTIONS

static bool ht_exclude (const char *value, const char **start, const char **end)
{
    bool exists = false;
    for (const char **iter = start; iter != end; ++iter)
    {
        if (*iter == NULL)
            continue;
        bool found = (strcmp (*iter, value) == 0);
        if (found && !exists)
        {
            exists = true;
            *iter = NULL;
        }
    }
    return exists;
}

bool ht_testEquality (HashTable *ht, const char **start, const char **end)
{
    bool ok = true;
    for (int i = 0; i < ht->hashsize; ++i)
    {
        if (ht->table[i].data != NULL)
            ok = ok && ht_exclude (ht->table[i].data, start, end);
        
        for (TableEl *iter = ht->table[i].next; iter != NULL; iter = iter->next)
            ok = ok && ht_exclude (iter->data, start, end);
    }

    for (const char **iter = start; iter != end; ++iter)
    {
        ok = ok && (*iter == NULL);
    }

    return ok;
}
