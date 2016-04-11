#include "WELL1024a.h"
#include <stdio.h>

#define MASK32  0xffffffffU
#define JMAX 32
static unsigned int B[JMAX];


static void init (unsigned int *A)
{
   int i;
   A[0] = 123456789;
   for (i = 1; i < JMAX; i++)
      A[i] = (663608941 * A[i - 1]) & MASK32;
}



int main (void)
{
   int i;
   int n = 1000000;

   init (B);
   InitWELLRNG1024a (B);
   for (i = 0; i < n; i++) {
      printf ("%10.8f  ", WELLRNG1024a()); 
      if (i%5==4) printf("\n"); 
   }

   return 0;
}