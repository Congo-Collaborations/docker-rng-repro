/*************************************************************************/
/*************************************************************************/
/*           Parallel Multiplicative Lagged Fibonacci Generator          */
/*                                                                       */ 
/* Modifed by: J. Ren                                                    */
/*             Florida State University                                  */
/*             Email: ren@csit.fsu.edu                                   */
/*                                                                       */
/*Additional Modifications by: W. John Thrasher                          */                                          
/*				Florida State University                                 */
/*				wjt1321@my.fsu.edu                                       */
/*                                                                       */
/* Based on the implementation by:                                       */
/*             Ashok Srinivasan (1997)                                   */
/*************************************************************************/
/*************************************************************************/

#ifndef _mlfg_h
#define _mlfg_h

#ifdef __cplusplus
extern "C" {
#endif

int * init_rng(int seed, int param); /* Initializes RNG w/ seed and parameter */
int get_rn_int(int * igenptr); /* Returns integer value from igenptr */
float get_rn_flt(int *igenptr); /* Returns floating-point value from igenptr */
double get_rn_dbl(int *igenptr); /* Returns double-precision floating-point value from igenptr */
void free_rng(int *genptr); /* Frees memory for data structure associated with stream */


#ifdef __cplusplus
}
#endif


#endif
