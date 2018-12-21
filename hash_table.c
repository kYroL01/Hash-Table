/**
   Generic implementation of Hash Table in C
   © Michele Campus (mcampus@qxip.net)
**/

#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"


/* Empty element */
static ht_item HT_DELETED_ITEM = {NULL, NULL};


/******* ADD *******/
/**
   #Internal
   * Given a key and a value, return a new item
   @param char* k: key
   @param char* v: value
   @return ht_item element
 **/
static ht_item* ht_new_item(const char* k, const char* v)
{
    ht_item* i = malloc(sizeof(ht_item));
    i->key = strdup(k);
    i->value = strdup(v);
    return i;
}


/**
   #Internal
   * Create a new Hash Table of a certain size
   @param int base_size: the initial size
   @return ht_hash_table the pointer to the new Hash Table
 **/

static ht_hash_table* ht_new_sized(const int base_size)
{
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    if(ht == NULL) return NULL;
    ht->base_size = base_size;
    ht->size = next_prime(ht->base_size);
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    if(ht->items == NULL) return NULL;

    return ht;
}

/** Create a new Hash Table of fixed size **/
ht_hash_table* ht_new()
{
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}


/******** REMOVE *********/
/**
   #Internal
   * Given the element, free it
   @param ht_item: the pointer to existing element
 **/
static void ht_del_item(ht_item* i)
{
    free(i->key);
    free(i->value);
    free(i);
}

/** Free memory allocated for the Hash Table **/
void ht_del_hash_table(ht_hash_table* ht)
{
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}


/****** HASH FUNCTION ******/
/**
   #Internal
   * This hash function has two steps:
   * 1) Convert the string to a large integer
   * 2) Reduce the size of the integer to a fixed range by taking its remainder mod m
   *
   * The variable 'a' should be a prime number larger than the size of the alphabet.
   * We're hashing ASCII strings, which has an alphabet size of 128,
   * so we should choose a prime larger than that.
   @param char* s: the string to be hashed
   @param int a: the prime integer
   @param int m: the module (number of buckets)
   @return the hash number of the string
**/
static int ht_hash(const char* s, const int a, const int m)
{
    long hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++) {
        hash += (long)pow(a, len_s - (i+1)) * s[i];
        hash = hash % m;
    }
    return (int) hash;
}

/**
   #Internal
   * Double hashing function to handling collitions
   * pseudo-code: index = (hash_a(string) + i*(hash_b(string) + 1)) % num_buckets
 **/
static int ht_get_hash(const char* s, const int num_buckets, const int attempt)
{
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}


/****** INSERT ******/
void ht_insert(ht_hash_table* ht, const char* key, const char* value)
{
    /**
       NOTE:
       To perform the resize, we check the load on the hash table on insert.
       If it is above predefined limits of 70 (0.7), resize up.
     **/
    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        ht_resize_up(ht);
    }

    // create new element to insert
    ht_item* item = ht_new_item(key, value);

    // get the first index of bucket and point it
    int index = ht_get_hash(item->key, ht->size, 0);
    ht_item* cur_item = ht->items[index];

    int i = 1;
    // loop untill find a free bucket
    while (cur_item != NULL) {
        /**
           NOTE: If we insert two items with the same key, the keys will collide,
           and the second item will be inserted into the next available bucket.
           When searching for the key, the original key will always be found,
           and we are unable to access the second item.
        **/
        if (cur_item != &HT_DELETED_ITEM) {
            if (strcmp(cur_item->key, key) == 0) {
                ht_del_item(cur_item);
                ht->items[index] = item;
                return;
            }
        }
        index = ht_get_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        i++;
    }
    ht->items[index] = item;
    ht->count++;
}


/****** SEARCH ******/
char* ht_search(ht_hash_table* ht, const char* key)
{
    // get the first index of bucket and point it
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];

    int i = 1;
    // loop untill elements exist
    while (item != NULL) {
        // if the element is not set as deleted and key match,
        // return the associated value
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                return item->value;
            }
        }
        // else, go ahead
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
    return NULL;
}


/****** DELETE ******/
void ht_delete(ht_hash_table* ht, const char* key)
{
    /**
       NOTE:
       To perform the resize, we check the load on the hash table on delete.
       If it is below predefined limits of 10 (0.1), resize down.
    **/
    const int load = ht->count * 100 / ht->size;
    if (load < 10) {
        ht_resize_down(ht);
    }

    // get the first index of bucket and point it
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];

    int i = 1;
    // loop untill elements exist
    while (item != NULL) {
        // if element exists and it's not already deleted, set pos as deleted
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
    ht->count--;
}


/****** RESIZE ******/
/**
   #Internal
   * Resize the Hash Table by the new size number
   @param ht: the Hash Table to resize
   @param base size: the new dimension
 **/
static void ht_resize(ht_hash_table* ht, const int base_size)
{
    if(base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }

    // new Hash Table used as temporary
    ht_hash_table* new_ht = ht_new_sized(base_size);

    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, item->key, item->value);
        }
    }

    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    // To delete new_ht, we give it ht's size and items
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    // now new_ht point to old items, so delete it
    ht_del_hash_table(new_ht);
}

/**
   #Internal
   * Resize UP. We need more space
 **/
static void ht_resize_up(ht_hash_table* ht)
{
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}

/**
   #Internal
   * Resize DOWN. We need less space
 **/

static void ht_resize_down(ht_hash_table* ht)
{
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}
