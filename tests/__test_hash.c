#include <assert.h>
#include <stdio.h>

#include "pih_lib.h"
#include "tests.h"

int __test_hash(void)
{
	hash_table_t hash = NULL;

	hash = hash_table_create();

	hash_table_add(hash, "felipe", "bacana");
	hash_table_add(hash, "yeah", "bacana");
	hash_table_add(hash, "uki", "duki");
	hash_table_add(hash, "felipe", "mongo");
	hash_table_add(hash, "xuxa", "querida");
	hash_table_add(hash, "lumpi", "mongo");

	iterator_t iterator = hash_table_iterator(hash);

	cell_t cell;
	char *value;

	fprintf(stderr, "-------------------------------------\n");
	while (iterator->has_next(iterator)) {
		cell = iterator->next(iterator);
		value = (char *) hash_table_get(hash, cell->id);

		fprintf(stderr, "%s %s\n", cell->id, (char *) value);
	}
        
    char *c = (char *) hash_table_get(hash, "felipe");
    assert(compare_strings(c, "mongo") == 0);
    c = (char *) hash_table_get(hash, "yeah");    
    assert(compare_strings(c, "bacana") == 0);
    c = (char *) hash_table_get(hash, "uki");
    assert(compare_strings(c, "duki") == 0);
    c = (char *) hash_table_get(hash, "xuxa");
    assert(compare_strings(c, "querida") == 0);
    c = (char *) hash_table_get(hash, "lumpi");
    assert(compare_strings(c, "mongo") == 0);
    

    list_iterator_destroy(&iterator);
    hash_table_destroy(&hash);

	return 0;
}
