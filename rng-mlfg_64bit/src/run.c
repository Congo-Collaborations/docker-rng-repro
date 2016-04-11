 #include <stdio.h>
 #include "mlfg.h"

int main(void)
{
  int seed, *gen, i;
  double rn;  
  
  seed = 985456376;
  
  gen = init_rng(seed, 0);

  /************************ print random numbers ***************************/
  
  for (i=0;i<1000000;i++)
  {
    rn = get_rn_dbl(gen);		/* generate double precision random number */
    printf("%f\n", rn);
  }

  
  
  free_rng(gen);		/* free memory used to store stream state  */
  return 0;
}
