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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NDEBUG
#include <assert.h>
#include <limits.h>
#include "mlfg.h"


#define NPARAMS 11

/* Handle 64-bit arithmetic */

#if LONG_MAX > 2147483647L 
#if LONG_MAX > 35184372088831L 
#if LONG_MAX >= 9223372036854775807L 
#define LONG_DEF
#define LONG64 long		/* 64 bit long */
#endif
#endif
#endif

#if !defined(LONG_DEF) &&  defined(_LONG_LONG)
#define LONG64 long long
#endif

#ifdef LONG64

typedef unsigned LONG64 uint64;

#ifdef LONG_DEF
#define MINUS1 0xffffffffffffffffUL		/* -1 (mod 2^(BITS-2)) */
#define ONE    0x1UL
#define MASK64 0xffffffffffffffffUL
#else
#define MINUS1 0xffffffffffffffffULL		/* -1 (mod 2^(BITS-2)) */
#define ONE    0x1ULL
#define MASK64 0xffffffffffffffffULL
#endif

#define multiply(a,b,c) {c = (a)*(b); c &= MASK64;}
#define add(a,b,c)      {c = (a)+(b); c &= MASK64;}
#define decrement(a,c)  {c = (a)-1; c &= MASK64;}
#define and(a,b,c)      {c = (a)&(b);}
#define or(a,b,c)       {c = (a)|(b);}
#define xor(a,b,c)      {c = (a)^(b);}
#define notzero(a)      (a==0?0:1)
#define lshift(a,b,c)   {c = (a)<<(b); c &= MASK64;} /* b is an int */
#define rshift(a,b,c)   {c = (a)>>(b); c &= MASK64;} /* b is an int */
#define highword(a)     ((unsigned int)((a)>>32))
#define lowword(a)      ((unsigned int)((a)&0xffffffff))
#define set(a,b)        {b = (a)&MASK64;}
#define seti(a,b)       {b = (a)&MASK64;} /* b is an int */
#define seti2(a,b,c)    {c = (b); c <<= 32; c |= (a); c &= MASK64;}/*a,b=+int*/

#else   /* Simulate 64 bit arithmetic on 32 bit integers */

typedef unsigned int uint64[2];

static const uint64 MASK64={0xffffffffU,0xffffffffU};
static const uint64 MINUS1={0xffffffffU,0xffffffffU}; /* -1 (mod 2^(BITS-2)) */
static uint64 ONE={0x1U,0x0U}; 
#define TWO_M32 2.3283064365386962e-10 /* 2^(-32) */

#define and(a,b,c)      {c[0] = a[0]&b[0]; c[1] = a[1]&b[1];}
#define or(a,b,c)       {c[0] = a[0]|b[0]; c[1] = a[1]|b[1];}
#define xor(a,b,c)      {c[0] = a[0]^b[0]; c[1] = a[1]^b[1];}
#define notzero(a)      ((a[0]==0 && a[1]==0)?0:1)
#define multiply(a,b,c) {c[1] = a[0]*b[1]+a[1]*b[0];\
                           c[1] += (unsigned int) (((double)a[0]*(double)b[0])\
                                                                *TWO_M32);\
                           c[0] = a[0]*b[0]; and(c,MASK64,c);}
#define add(a,b,c)      {unsigned int t = a[0]+b[0]; \
                           c[1] = a[1]+b[1]+(t<a[0]);c[0]=t;\
                           and(c,MASK64,c);}
#define decrement(a,c)  {if(a[0]==0){c[1]=a[1]-1;c[0]=0xffffffff;} \
                            else c[0] = a[0]-1; and(c,MASK64,c);}

static void lshift(uint64 a,int b,uint64 c)   
{
  if(b<32)
  {c[1] = (a[1]<<b)|(a[0]>>(32-b)); c[0] = a[0]<<(b);}
  else {c[1]=a[0]<<(b-32);c[0]=0;} 
  and(c,MASK64,c);
} 

static void rshift(uint64 a,int b,uint64 c)   
{
  if(b<32)
  {c[0] = (a[0]>>b)|(a[1]<<(32-b));c[1] = a[1]>>(b);}
  else {c[0]=a[1]>>(b-32);c[1]=0;} 
  and(c,MASK64,c);
} 

#define highword(a)     ((a)[1])
#define lowword(a)      ((a)[0])
#define set(a,b)        {b[0] = a[0];b[1]=a[1];and(b,MASK64,b);}
#define seti(a,b)       {b[0] = a;b[1]=0;} /* b is an int */
#define seti2(a,b,c)    {c[1] = b; c[0] = a; and(c,MASK64,c);}/*a,b = +ve int*/

#endif  /* LONG64 or 32 bit */  



#define TWO_M52 2.2204460492503131e-16 /* 2^(-52) */
#define TWO_M64 5.4210108624275222e-20 /* 2^(-64) */
#define BITS 62			/* Initialization of ALFG part is m-2 bits */
#define MAX_BIT_INT (BITS-2)
#define RUNUP (2*BITS)		/* Do RUNUP iterations after initialization */
#define GS0 0x372f05ac

#ifdef LONG64
#define INT_MOD_MASK (MASK64>>(64-BITS))
#define INT_MASK (MASK64>>(64-BITS+1))
#define INTX2_MASK ((((uint64)1)<<MAX_BIT_INT)-1)
static const uint64 SEED_MASK=0x5a38;
#else
static const uint64 INT_MOD_MASK={0xffffffffU,0x3fffffffU};
static const uint64 INT_MASK={0xffffffffU,0x1fffffffU};
static const uint64 INTX2_MASK={0xffffffffU,0x0fffffffU};
static const uint64 SEED_MASK={0x5a38,0x0U};
#endif

struct rngen
{ 
  int init_seed;
  int parameter;
  int narrays;
  int *array_sizes;
  int **arrays;

  uint64 *lags;
  uint64 *si;
  int hptr;          /* integer pointer into fill */
  int lval, kval, seed;
};

struct vstruct {
      int L;
      int K;
      int LSBS;     /* number of least significant bits that are 1 */
      int first;    /* the first seed whose LSB is 1 */
};

const struct vstruct valid[] = { {17,5,1,10}, {31,6,1,2},
{55,24,1,11}, {63,31,1,14}, {127,97,1,21}, {521,353,1,100},
{521,168,1,83}, {607,334,1,166}, {607,273,1,105}, {1279,418,1,208}, {1279,861,1,233} };

/*************************************************************************/
/*************************************************************************/
/*            ROUTINES USED TO CREATE GENERATOR FILLS                    */
/*************************************************************************/
/*************************************************************************/

static int bitcnt( int x)
{
  unsigned i=0,y;

  for (y=(unsigned)x; y; y &= (y-1) ) 
    i++;

  return(i);
}



static void advance_reg(int *reg_fill)
{
/*      the register steps according to the primitive polynomial         */
/*           (64,4,3,1,0); each call steps register 64 times             */
/*      we use two words to represent the register to allow for integer  */
/*           size of 32 bits                                             */

  const int mask = 0x1b;
  int adv_64[4][2];
  int i,new_fill[2];
  unsigned temp;

  adv_64[0][0] = 0xb0000000;
  adv_64[0][1] = 0x1b;
  adv_64[1][0] = 0x60000000;
  adv_64[1][1] = 0x2d;
  adv_64[2][0] = 0xc0000000;
  adv_64[2][1] = 0x5a;
  adv_64[3][0] = 0x80000000;
  adv_64[3][1] = 0xaf;
  new_fill[1] = new_fill[0] = 0;
  temp = mask<<27;

  for (i=27;i>=0;i--) 
  {
    new_fill[0] = (new_fill[0]<<1) | (1&bitcnt(reg_fill[0]&temp));
    new_fill[1] = (new_fill[1]<<1) | (1&bitcnt(reg_fill[1]&temp));
    temp >>= 1;
  }

  for (i=28;i<32;i++) 
  {
    temp = bitcnt(reg_fill[0]&(mask<<i));
    temp ^= bitcnt(reg_fill[1]&(mask>>(32-i)));
    new_fill[0] |= (1&temp)<<i;
    temp = bitcnt(reg_fill[0]&adv_64[i-28][0]);
    temp ^= bitcnt(reg_fill[1]&adv_64[i-28][1]);
    new_fill[1] |= (1&temp)<<i;
  }

  reg_fill[0] = new_fill[0];
  reg_fill[1] = new_fill[1];
}



static void get_fill(uint64 *n, uint64 *r, int param, unsigned seed)
{
  int i,j,k,temp[2], length;
  uint64 tempui;
  
  length = valid[param].L;
  
  /* Initialize the shift register with the node number XORed with seed    */
  temp[1] = highword(n[0]);
  temp[0] = lowword(n[0])^seed;
  if (!temp[0])
    temp[0] = GS0;


  advance_reg(temp); /* Advance the shift register some */
  advance_reg(temp);

  /* The first word of the RNG is defined by the LSBs of the node number   */
  and(n[0],INT_MASK,tempui);
  lshift(tempui,1,r[0]);
  
  /* The RNG is filled with the bits of the shift register, at each time   */
  /* shifted up to make room for the bits defining the canonical form;     */
  /* the node number is XORed into the fill to make the generators unique  */

  for (i=1;i<length-1;i++) 
  {
    advance_reg(temp);

    seti2(temp[0],temp[1],tempui);
    xor(tempui,n[i],tempui);
    and(tempui,INT_MASK,tempui);
    lshift(tempui,1,r[i]);
  }
  seti(0,r[length-1]);
/*      the canonical form for the LSB is instituted here                */
  k = valid[param].first + valid[param].LSBS;

  for (j=valid[param].first;j<k;j++)
    or(r[j],ONE,r[j]);

  return;
}


/* left shift array 'b' by one, and place result in array 'a' */
static void si_double(uint64 *a,  uint64 *b, int length)
{
  int i;
  uint64 mask1, temp1;
  
  lshift(ONE,MAX_BIT_INT,mask1);
  
  and(b[length-2],mask1,temp1)
  if (notzero(temp1))
    fprintf(stderr,"WARNING: si_double -- RNG has branched maximum number of times.\n\t Independence of generators no longer guaranteed\n");

  and(b[length-2],INTX2_MASK,temp1);
  lshift(temp1,1,a[length-2]);

  for (i=length-3;i>=0;i--) 
  {
    and(b[i],mask1,temp1)
    if(notzero(temp1)) 
      add(a[i+1],ONE,a[i+1]);

    and(b[i],INTX2_MASK,temp1);
    lshift(temp1,1,a[i]);
  }
}



static void pow3(uint64 n, uint64 *ui)		/* return 3^n (mod 2^BITS) */
{
  uint64 value, temp, bit, temp2, temp3; /*p*/
  int exponent;
  
  /*set(n,p);*/
  seti(3,temp);
  seti(1,temp3);
  and(n,temp3,temp2);
  if(notzero(temp2))
    seti(3,value)
  else
    seti(1,value)
  seti(1,bit);
  
  for(exponent=2; exponent<64; exponent++)
  {
    multiply(temp,temp,temp);
    lshift(bit,1,bit);
    
    and(bit,n,temp2);
    if(notzero(temp2))
      multiply(value,temp,value);
  }
  
  and(value,MASK64,value);
  
  set(value,(*ui));
}


static void findseed(int sign, uint64 n, uint64 *ui)
{
  uint64 temp;
  
  pow3(n,&temp);
  
  if(sign&1)
    multiply(temp,MINUS1,temp);
  
  set(temp,(*ui));
}


#define  advance_state(genptr)  {int lval = genptr->lval, kval = genptr->kval;\
  int lptr;\
  genptr->hptr--;\
  if(genptr->hptr < 0)\
   genptr->hptr = lval-1;\
  lptr = genptr->hptr + kval;\
  if (lptr>=lval)\
   lptr -= lval;\
  multiply(genptr->lags[genptr->hptr],genptr->lags[lptr],genptr->lags[genptr->hptr]);}

static struct rngen *initialize(int param, unsigned int seed, 
				 uint64 *nstart, unsigned int initseed)
{
  int i,j,k,m, length; /*i,j,k,l,m, length;*/
  struct rngen *q;
  uint64 temp1, mask; /* *nindex */
  length = valid[param].L;

  q = (struct rngen *) malloc(sizeof(struct rngen));
  if (q == NULL) 
    return NULL;

  q->hptr = 0;   /* This is reset to lval-1 before first iteration */
  q->si = (uint64 *) malloc((length-1)*sizeof(uint64));
  q->lags = (uint64 *) malloc(length*sizeof(uint64));
  q->lval = length;
  q->kval = valid[param].K;
  q->parameter = param;
  q->seed = seed;
  q->init_seed = initseed;
  q->narrays=2;
  
  if (q->lags == NULL || q->si == NULL) 
    return NULL;

/*      specify register fills and node number arrays                    */
/*      do fills in tree fashion so that all fills branch from index     */
/*           contained in nstart array                                   */
  //q->stream_number = lowword(nstart[0]);
  get_fill(nstart,q->lags,param,seed);
  si_double(q->si,nstart,length);
  
  set(ONE,mask);
  for(m=0; m<length; m++)
  {
    and(SEED_MASK,mask,temp1);
    if(notzero(temp1))
      findseed(1,q->lags[m], &q->lags[m]);
    else
      findseed(0,q->lags[m], &q->lags[m]);
    lshift(mask,1,mask);
  }
  	
	add(q->si[0],ONE,q->si[0]);
  	
	i = 0;
  
  k = 0;
  for (j=1;j<length-1;j++) {
    if (notzero(q->si[j])) 
      k = 1; 
    }
    if (k) {
      for (j=0;j<length*RUNUP;j++)
        advance_state(q);
    }
  while (i>=0)
  {
    for (j=0;j<4*length;j++)
      advance_state(q);
    i--;
  } 
  
  return q;
}

/* Initialize random number stream */
int *init_rng(int seed, int param)
{
/*      gives back one stream (node gennum) with updated spawning         */
/*      info; should be called total_gen times, with different value      */
/*      of gennum in [0,total_gen) each call                              */
  struct rngen *genptr = NULL;//*p=NULL, *genptr;
  uint64 *nstart=NULL; //,*si;
  int i, length; //, k;

  seed &= 0x7fffffff;		/* Only 31 LSB of seed considered */

  if (param < 0 || param >= NPARAMS)     /* check if parameter is valid */
  {
    fprintf(stderr,"WARNING - init_rng: parameter not valid. Using Default parameter.\n");
    param = 0;
  }
  
  length = valid[param].L; /* determine parameters   */  
  
/*      define the starting vector for the initial node                  */
  nstart = (uint64 *) malloc((length-1)*sizeof(uint64));
  if (nstart == NULL)
    return NULL;

  seti(0,nstart[0]);
  for (i=1;i<length-1;i++) 
    seti(0,nstart[i]);

  genptr = initialize(param,seed^GS0,nstart,seed);  /* create a generator  */
  if (genptr==NULL) 
    return NULL;
  
  genptr->array_sizes = (int *) malloc(genptr->narrays*sizeof(int));
  genptr->arrays = (int **) malloc(genptr->narrays*sizeof(int *));
  if(genptr->array_sizes == NULL || genptr->arrays == NULL)
    return NULL;
  genptr->arrays[0] = (int *) genptr->lags;
  genptr->arrays[1] = (int *) genptr->si;
  genptr->array_sizes[0] = genptr->lval*sizeof(uint64)/sizeof(int);
  genptr->array_sizes[1] = (genptr->lval-1)*sizeof(uint64)/sizeof(int);
  
  free(nstart);
  
  return (int *) genptr;
} 

/* Returns a double precision random number */

double get_rn_dbl(int *igenptr)
{
  struct rngen *genptr = (struct rngen *) igenptr;

  advance_state(genptr);	

#ifdef LONG64  
  return (genptr->lags[genptr->hptr]>>12)*TWO_M52;
#else
  return (genptr->lags[genptr->hptr][1])*TWO_M32 + 
    (genptr->lags[genptr->hptr][0])*TWO_M64;
#endif
} 

/* Return a random integer */

int get_rn_int(int *igenptr)
{
  struct rngen *genptr = (struct rngen *) igenptr;

  advance_state(genptr);	

#ifdef LONG64
  return genptr->lags[genptr->hptr]>>33;
#else
  return genptr->lags[genptr->hptr][1]>>1;
#endif
} 

/* Return a single precision random number */

float get_rn_flt(int *igenptr)
{
    return (float) get_rn_dbl(igenptr);
}

/* Free memory allocated for data structure associated with stream */

void free_rng(int *genptr)
{
  struct rngen *q;
  int i;
  
  q = (struct rngen *) genptr;
  assert(q != NULL);
  
  for(i=0; i<q->narrays; i++)
    free(q->arrays[i]);

  if(q->narrays > 0)
  {
    free(q->array_sizes);
    free(q->arrays);
  }
  
  free(q);
}
