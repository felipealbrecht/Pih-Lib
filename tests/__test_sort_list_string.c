#include <stdio.h>


#include "pih_lib.h"

int main()
{

	list_t un = list_create();
	list_add(un, "felipe", NULL);
	list_add(un, "manu", NULL);
	list_add(un, "lumpi", NULL);
	list_add(un, "adao", NULL);
	list_add(un, "zebra", NULL);
	list_add(un, "abrao", NULL);
	list_add(un, "rio", NULL);
	list_add(un, "xaxin", NULL);

	list_print(un, print_dummy);

	fprintf(stderr, "\n\n");

	list_t sorted = list_sort(un, compare_strings);
        free(un);

        list_print(sorted, print_dummy);

        list_destroy(&sorted);

	return 0;
}
