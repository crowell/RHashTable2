#include <r_util.h>
#include "ht.h"

void free_kvp (pair_t* kvp) {
	free (kvp->key);
	free (kvp->value);
	free (kvp);
}

int main (void) {
	RHashTable2* ht;
	ht = r_ht_new (r_str_hash64, strcmp, strdup, strdup, free_kvp);
	r_ht_insert (ht, "bc", "A");
	r_ht_insert (ht, "cB", "B");
	r_ht_insert (ht, "c", "C");
	bool found;
	char* str = r_ht_find (ht, "bc", &found);
	printf ("%s\n", str);
	str = r_ht_find (ht, "cB", &found);
	printf ("%s\n", str);
	str = r_ht_find (ht, "c", &found);
	printf ("%s\n", str);
	r_ht_destroy (ht);
	return 0;
}
