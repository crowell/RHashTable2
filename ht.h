/**
 * RHashTable2 - A smart, 'templatized' pure-c hashtable for use in radare2,
 * sdb, etc. RHashTable2 is an auto-resizing hash table where the variables
 * can be tuned to accomodate any type of object, while keeping collision
 * resolution.
 * Of course INSERT, DELETE, FIND are all done in O(1).
 * radare2 - BSD License - 2016 - crowell
 **/

#ifndef R_HT_H
#define R_HT_H

#include <r_util.h>

typedef void* (*RDupKey)(void* key);
typedef void* (*RDupValue)(void* value);
typedef ut64 (*RHashFunction)(void* value);

typedef struct pair_t{
	void* key;
	void* value;
	struct r_ht_entry* next;
} pair_t;

typedef struct r_ht {
	ut64 size; // size of the hash table in buckets.
	RListComparator* cmp; // Function for comparing values. Returns 0 if eq.
	RHashFunction hashfn; // Function for hashing items in the hash table.
	RDupKey dupkey; // Function for making a copy of key to store in the entry. NULL for just copying the pointer.
	RDupValue dupvalue; // Function for making a copy of value to store in the entry. NULL for just copying the pointer.
	RList/*<pair_t>*/** table;  // Actual table.
} r_ht;
typedef r_ht RHashTable2;

// Create a new RHashTable2.
// RHashFunction - the function pointer to the hash function. For a string, use r_str_hash64 for dbj2. Must not be NULL.
// comparitor - the function pointer to comparison function. Returns 0 for equality eg) for a string strcmp.
// keydup - the function pointer to a dup function. eg) for a string strdup, if NULL is provided, just assignment of the value/pointer
// valdup - the function pointer to a dup function. eg) for a string strdup, if NULL is provided, just assignment of the value/pointer
// pair_free - the function pointer to the function to free a pair_t entry, if NULL is provided, just free is used.
R_API r_ht* r_ht_new(RHashFunction hashfunction, RListComparator comparitor,
	RDupKey keydup, RDupValue valdup, RListFree pair_free);

// Destroy a hashtable and all of its entries.
R_API void r_ht_destroy(RHashTable2* ht);

#endif // R_HT_H