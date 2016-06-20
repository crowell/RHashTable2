#include <r_util.h>
#include "ht.h"

int main (void) {
	RHashTable2* ht;
	ht = r_ht_new (r_str_hash64, strcmp, strdup, strdup, NULL);
	r_ht_insert (ht, "a", "A");
	r_ht_insert (ht, "b", "B");
	r_ht_insert (ht, "c", "C");
	bool found;
	char* str = r_ht_find (ht, "a", &found);
	printf ("%s\n", str);
	return 0;
}
