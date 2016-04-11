#ifndef WELL1024A_H
#define WELL1024A_H

/*************************************************************************
Copyright:  Francois Panneton and Pierre L'Ecuyer, Université de Montréal
            Makoto Matsumoto, Hiroshima University

Notice:     This code can be used freely for personal, academic,
            or non-commercial purposes. For commercial purposes,
            please contact P. L'Ecuyer at: lecuyer@iro.UMontreal.ca

   This code can also be used under the terms of the GNU General Public
   License as published by the Free Software Foundation, either version 3
   of the License, or any later version. See the GPL licence at URL
   http://www.gnu.org/licenses

**************************************************************************/

/* seed is an array of 32 unsigned int's used as seed of the generator */
void InitWELLRNG1024a (unsigned int *seed);

double WELLRNG1024a (void);

#endif