/**
   Generic implementation of Hash Table in C
**/

 #include <stdio.h>
 #include <stdlib.h

#define HT_INITIAL_BASE_SIZE 50
#define HT_PRIME_1           131
#define HT_PRIME_2           151


// Items
typedef struct {
    char* key;
    char* value;
} ht_item;


// Hash Table
typedef struct {
    int base_size;
    int size;
    int count;
    ht_item** items;
} ht_hash_table;



/**
   Create a new Hash Table of fixed size
   @return ht_hash_table Hash Table
 **/
ht_hash_table* ht_new();

/**
   Free memory allocated for the Hash Table
   @param ht_hash_table: the pointer to the HT
 **/
void ht_del_hash_table(ht_hash_table* ht);

/**
   Insert a new key-value pair element in the Hash Table
   @param ht_hash_table* ht: the Hash Table
   @param char* key: the key
   @param char* value: the value
 **/
void ht_insert(ht_hash_table* ht, const char* key, const char* value);

/**
   Search an element by its key in the Hash Table
   @param ht_hash_table* ht: the Hash Table
   @param char* key: the key
   @param char* value: the value
   @return the item value associated to key
 **/
char* ht_search(ht_hash_table* ht, const char* key);

/**
   Delete an element searching by its key in the Hash Table
   NOTE: because of double hashing for handling collision,
         instead of deleting the item, it simply mark it as deleted.
   @param ht_hash_table* ht: the Hash Table
   @param char* key: the key
   @param char* value: the value
 **/
void ht_delete(ht_hash_table* h, const char* key);
