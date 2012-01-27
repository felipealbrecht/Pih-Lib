#include <assert.h>
#include <stdio.h>

#include "pih_lib.h"

int __test_hash()
{
	hash_table_t hash = NULL;

	hash = hash_table_create();

	hash_table_add(hash, "felipe", "bacana");
	hash_table_add(hash, "yeah", "bacana");
	hash_table_add(hash, "uki", "duki");
	hash_table_add(hash, "felipe", "mongo");
	hash_table_add(hash, "manu", "querida");
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

        list_iterator_destroy(&iterator);
        hash_table_destroy(&hash);

	return 0;
}
