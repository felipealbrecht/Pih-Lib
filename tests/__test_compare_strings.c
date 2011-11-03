#include <assert.h>
#include <stdio.h>

#include "pih_lib.h"


int main()
{
	assert( compare_strings("aaaaa", "bbbb") == -1);
	assert( compare_strings("bbbb", "aaaaa") == 1);
	assert( compare_strings("1", "2") == -1);
	assert( compare_strings("2", "1") == 1);
	assert( compare_strings("qet", "qea") == 1);
	assert( compare_strings("aaaa", "aaaa") == 0);
	assert( compare_strings("aaaaaaaa", "aaaa") == 1);
	assert( compare_strings("aaaaaaaa", "aaaaaaaaaaaa") ==-1);
	assert( compare_strings("b", "aaaaaaaaaaaa") == 1);
	assert( compare_strings("a", "aaaaaaaaaaaa") == -1);
	assert(compare_strings("aaaaaaaaaaa", "a") == 1);
	assert(compare_strings("", "aaa") == -1);
	assert(compare_strings("bbb", "") == 1);
	assert(compare_strings("a", "") == 1);
	assert(compare_strings("", "") == 0);
	assert(compare_strings("felipe", "joao") == -1);
	assert(compare_strings("b", "c") == -1);
	assert(compare_strings("d", "c") == 1);
	assert(compare_strings("dormir", "dormir") == 0);
	assert(compare_strings("felipe","manu") == -1);
	assert(compare_strings("felipe","lumpi") == -1);
	assert(compare_strings("felipe","adao") == 1);
	assert(compare_strings("lumpi","adao") == 1);
	assert(compare_strings("zebra","abrao") == 1);
	assert(compare_strings("felipe","abrao") == 1);
	assert(compare_strings("lumpi","abrao") == 1);
	assert(compare_strings("adao","abrao") == 1);
	assert(compare_strings("zebra","rio") == 1);
	assert(compare_strings("felipe","rio") == -1);
	assert(compare_strings("lumpi","rio") == -1);
	assert(compare_strings("adao","rio") == -1);
	assert(compare_strings("abrao","rio") == -1);
	assert(compare_strings("zebra","xaxin") == 1);
	assert(compare_strings("felipe","xaxin") == -1);
	assert(compare_strings("lumpi","xaxin") == -1);

	fprintf(stdout, "Cheer! OutStanding!!!\n");

	return 0;
}
