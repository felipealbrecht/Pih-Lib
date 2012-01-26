#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "pih_lib.h"

typedef struct {
} X;

int main()
{
   setlevel(OUT_SHOW_ALL);

   char *s = (char *) malloc(sizeof(char) * 10);
   memset(s, '\0', sizeof(char) * 10);

   memcpy(s, "pih", 3);

   fprintf(stderr, "%s\n", s);

   X* t1 = (X*) malloc(sizeof(X));
   X* t2 = (X*) malloc(sizeof(X));
   X* t3 = (X*) malloc(sizeof(X));

   shared_ptr_t shr = shared_ptr_create(s, t1);
   fprintf(stderr, "shr: %p\n", shr);
   shared_ptr_inc_ref(shr, t2);
   shared_ptr_inc_ref(shr, t3);

   fprintf(stderr, "shr has %ld references\n", shr->refs->size);

   shared_ptr_rem_ref(&shr, t2);
   shared_ptr_rem_ref(&shr, t1);
   shared_ptr_rem_ref(&shr, t3);

   fprintf(stderr, "shr: %p\n", shr);
}
