//
//  main.c
//  PihLib
//
//  Created by Felipe Albrecht on 1/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>

#include "pih_lib.h"
#include "tests.h"

int main (int argc, const char * argv[])
{
    __test_compare_strings();
    __test_hash();
    __test_shared_ref();
    __test_sort_list_string();
    
    
    fprintf(stdout, "All tests passed!\n");
    
    return 0;
}

