#include "ht.h"

static RHashTable2* internal_ht_new(ut64 size, RHashFunction hashfunction,
		RListComparator comparator, RDupKey keydup, RDupValue valdup,
		RListFree pair_free) {
	RHashTable2* ht;
	int i;
	ht = calloc (1, sizeof(*ht));
	if (ht != NULL) {
		ht->size = size;
		ht->count = 0;
		ht->load_factor = 1;
		ht->hashfn = hashfunction;
		ht->cmp = comparator;
		ht->dupkey = keydup;
		ht->dupvalue = valdup;
		ht->table = calloc (ht->size, sizeof (RList*));
		if (ht->table != NULL) {
			for (i = 0; i < ht->size; ++i) {
				if (pair_free) {
					ht->table[i] = r_list_newf (pair_free);
				} else {
					ht->table[i] = r_list_new ();
				}
			}
		} else {
			free (ht);
			return NULL;
		}
	}
	return ht;
}

R_API RHashTable2* r_ht_new(RHashFunction hashfunction, RListComparator comparator,
		RDupKey keydup, RDupValue valdup, RListFree pair_free) {
	ut64 initial_size = 1024;
	return internal_ht_new (initial_size, hashfunction, comparator, keydup,
			valdup, pair_free);
}

R_API void r_ht_destroy(RHashTable2* ht) {
	int i;
	for (i = 0; i < ht->size; ++i) {
		r_list_free (ht->table[i]);
	}
	free (ht->table);
	free (ht);
}

// Increases the size of the hashtable by 2.
static void internal_ht_grow(RHashTable2* ht) {
	RHashTable2* ht2;
	RHashTable2 swap;
	pair_t* kvp;
	RListIter* iter;
	int i;
	ht2 = internal_ht_new (ht->size * 2, ht->hashfn, ht->cmp, ht->dupkey,
			ht->dupvalue, ht->table[0]->free);
	for (i = 0; i < ht->size; ++i) {
		r_list_foreach (ht->table[i], iter, kvp) {
			(void)r_ht_insert (ht2, kvp->key, kvp->value);
		}
	}
	// And now swap the internals.
	swap = *ht;
	*ht = *ht2;
	*ht2 = swap;
	r_ht_destroy (ht2);
}

// Inserts the key value pair key, value into the hashtable.
// if update is true, allow for updates, otherwise return false if the key
// already exists.
bool internal_ht_insert(RHashTable2* ht, bool update, void* key, void* value) {
	pair_t* kvp;
	ut64 hash;
	ut64 bucket;
	bool found = true;
	if (update) {
		r_ht_delete (ht, key);
	} else {
		(void)r_ht_find (ht, key, &found);
	}
	if (update || !found) {
		kvp = calloc (1, sizeof (pair_t *));
		if (kvp) {
			if (ht->dupkey) {
				kvp->key = ht->dupkey (key);
			} else {
				kvp->key = key;
			}
			if (ht->dupvalue) {
				kvp->value = ht->dupvalue (value);
			} else {
				kvp->value = value;
			}
			hash = ht->hashfn (key);
			bucket = hash % ht->size;
			r_list_prepend (ht->table[bucket], kvp);
			ht->count++;
			// Check if we need to grow the table.
			if (ht->count >= ht->load_factor * ht->size) {
				internal_ht_grow (ht);
			}
			return true;
		}
	}
	return false;
}

// Inserts the key value pair key, value into the hashtable.
// Doesn't allow for "update" of the value.
R_API bool r_ht_insert (RHashTable2* ht, void* key, void* value) {
	return internal_ht_insert (ht, false, key, value);
}

// Inserts the key value pair key, value into the hashtable.
// Does allow for "update" of the value.
R_API bool r_ht_update (RHashTable2* ht, void* key, void* value) {
	return internal_ht_insert (ht, true, key, value);
}

// Looks up the corresponding value from the key. Returns true if found, false
// otherwise.
R_API void* r_ht_find(RHashTable2* ht, void* key, bool* found) {
	ut64 hash;
	ut64 bucket;
	RListIter* iter;
	RList* list;
	pair_t* kvp;
	hash = ht->hashfn (key);
	bucket = hash % ht->size;
	r_list_foreach (ht->table[bucket], iter, kvp) {
		if (ht->cmp) {
			if (ht->cmp (key, kvp->key) == 0) {
				*found = true;
				return  kvp->value;
			}
		} else {
			if (key == kvp->key) {
				*found = true;
				return  kvp->value;
			}
		}
	}
	*found = false;
	return NULL;
}

// Deletes a kvp from the hash table from the key, if the pair exists.
R_API void r_ht_delete(RHashTable2* ht, void* key) {
	ut64 hash;
	ut64 bucket;
	RListIter* iter;
	RList* list;
	pair_t* kvp;
	hash = ht->hashfn (key);
	bucket = hash % ht->size;
	r_list_foreach (ht->table[bucket], iter, kvp) {
		if (ht->cmp) {
			if (ht->cmp (key, kvp->key) == 0) {
				r_list_delete (list, iter);
				return;
			}
		} else {
			if (key == kvp->key) {
				r_list_delete (list, iter);
				return;
			}
		}
	}
}
