#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "pih_lib.h"
#include "tests.h"

int __test_sort_list_string(void)
{

	list_t un = list_create();
	list_add(un, "felipe", NULL);
	list_add(un, "xuxa", NULL);
	list_add(un, "lumpi", NULL);
	list_add(un, "adao", NULL);
	list_add(un, "zebra", NULL);
	list_add(un, "abrao", NULL);
	list_add(un, "rio", NULL);
	list_add(un, "xaxin", NULL);


	list_print(un, print_dummy);

	fprintf(stderr, "\n\n");

	list_t sorted = list_sort(un, compare_by_id);
    list_destroy(&un);
    
    assert(strcmp(list_get_by_pos(sorted, 0), "abrao") == 0);
    assert(strcmp(list_get_by_pos(sorted, 1), "adao") == 0);
    assert(strcmp(list_get_by_pos(sorted, 2), "felipe") == 0);
    assert(strcmp(list_get_by_pos(sorted, 3), "lumpi") == 0);
    assert(strcmp(list_get_by_pos(sorted, 4), "rio") == 0);    
    assert(strcmp(list_get_by_pos(sorted, 5), "xaxin") == 0);
    assert(strcmp(list_get_by_pos(sorted, 6), "xuxa") == 0);
    assert(strcmp(list_get_by_pos(sorted, 7), "zebra") == 0);

    list_print(sorted, print_dummy);
    list_destroy(&sorted);

	return 0;
}
