#include <stdio.h>
#include "MRG32k3a.h"


int main(void)
{
    int i;
  
    printf("\n1000 outputs of MRG32k3a()\n");
    for (i=0; i<1000; i++) {
      printf("%10.8f ", MRG32k3a());
      if (i%5==4) printf("\n");
    }
    return 0;
}