/*=============================================================================

Copyright (C) 2007, 2008 David Harvey (zn_poly)
Copyright (C) 2013 William Hart

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <stdlib.h>
#include <gmp.h>
#include "flint.h"
#include "nmod_vec.h"
#include "nmod_poly.h"

void
_nmod_poly_KS2_reduce(mp_ptr res, slong s, mp_srcptr op, slong n, ulong w,
                     nmod_t mod)
{
   if (w == 1)
   {
      for (; n; n--, res += s, op++)
         NMOD_RED(*res, *op, mod);
   }
   else if (w == 2)
   {
      for (; n; n--, res += s, op += 2)
         NMOD2_RED2(*res, op[1], op[0], mod);
   }
   else /* w == 3 */
   {
      for (; n; n--, res += s, op += 3)
         NMOD_RED3(*res, op[2], op[1], op[0], mod);
   }
}

/*
   Same as _nmod_poly_KS2_recover_reduce(), but requires 0 < 2 * b <= FLINT_BITS
*/
void
_nmod_poly_KS2_recover_reduce1(mp_ptr res, slong s, mp_srcptr op1,
                          mp_srcptr op2, slong n, ulong b,
                          nmod_t mod)
{
   ulong mask = (1UL << b) - 1;

   /* (x0, x1) and (y0, y1) are two-digit windows into X and Y. */
   ulong x1, x0 = *op1++;
   ulong y0, y1, borrow;

   op2 += n;
   y1 = *op2--;

   borrow = 0;

   /* plain reduction version */
   for (; n; n--)
   {
      y0 = *op2--;
      x1 = *op1++;
      if (y0 < x0)
         y1--;
      NMOD_RED(*res, x0 + (y1 << b), mod);
      res += s;
      y1 += borrow;
      borrow = (x1 < y1);
      x1 -= y1;
      y1 = (y0 - x0) & mask;
      x0 = x1 & mask;
   }
}


/*
   Same as _nmod_poly_KS2_recover_reduce(), but requires
   FLINT_BITS < 2 * b < 2*FLINT_BITS
*/
void
_nmod_poly_KS2_recover_reduce2(mp_ptr res, slong s, mp_srcptr op1,
                          mp_srcptr op2, slong n, ulong b,
                          nmod_t mod)
{
   /*
      The main loop is the same as in _nmod_poly_KS2_recover_reduce1(), but the
      modular reduction step needs to handle two input words.
   */
   ulong mask = (1UL << b) - 1;

   ulong x1, x0 = *op1++;
   ulong y0, y1, borrow, b2;

   op2 += n;
   y1 = *op2--;

   borrow = 0;
   
   b2 = FLINT_BITS - b;

   /* plain reduction version */
   for (; n; n--)
   {
      y0 = *op2--;
      x1 = *op1++;
      if (y0 < x0)
         y1--;
      NMOD2_RED2(*res, y1 >> b2, x0 + (y1 << b), mod);
      res += s;
      y1 += borrow;
      borrow = (x1 < y1);
      x1 -= y1;
      y1 = (y0 - x0) & mask;
      x0 = x1 & mask;
   }
}


/*
   Same as _nmod_poly_KS2_recover_reduce(), but requires b == FLINT_BITS
*/
void
_nmod_poly_KS2_recover_reduce2b(mp_ptr res, slong s, mp_srcptr op1,
                          mp_srcptr op2, slong n, ulong b,
                          nmod_t mod)
{
   /*
      Basically the same code as _nmod_poly_KS2_recover_reduce2(), specialised
      for b == FLINT_BITS.
   */
   ulong x1, x0 = *op1++;
   ulong y0, y1, borrow;

   op2 += n;
   y1 = *op2--;

   borrow = 0;

   /* plain reduction version */
   for (; n; n--)
   {
      y0 = *op2--;
      x1 = *op1++;
      if (y0 < x0)
         y1--;
      NMOD2_RED2(*res, y1, x0, mod);
      res += s;
      y1 += borrow;
      borrow = (x1 < y1);
      x1 -= y1;
      y1 = y0 - x0;
      x0 = x1;
   }
}

/*
   Same as _nmod_poly_KS2_recover_reduce(), but requires
   2 * FLINT_BITS < 2 * b <= 3 * FLINT_BITS.
*/
void
_nmod_poly_KS2_recover_reduce3(mp_ptr res, slong s, mp_srcptr op1,
                          mp_srcptr op2, slong n, ulong b,
                          nmod_t mod)
{
   /* 
      The main loop is the same as in zn_array_recover_reduce1(), but needs
      to operate on double-word quantities everywhere, i.e. we simulate
      double-word registers. The suffixes L and H stand for low and high words
      of each.
   */

   ulong maskH = (1UL << (b - FLINT_BITS)) - 1;

   ulong x1L, x0L = *op1++;
   ulong x1H, x0H = *op1++;
   ulong y0H, y1H, y0L, y1L;
   ulong borrow, b1, b2;

   op2 += 2 * n + 1;
   y1H = *op2--;
   y1L = *op2--;

   borrow = 0;

   b1 = b - FLINT_BITS;
   b2 = 2 * FLINT_BITS - b;

   /* plain reduction version */
   for (; n; n--)
   {
      y0H = *op2--;
      y0L = *op2--;
      x1L = *op1++;
      x1H = *op1++;
      if ((y0H < x0H) || (y0H == x0H  &&  y0L < x0L))
         y1H -= (y1L-- == 0);

      NMOD_RED3(*res, (y1H << b1) + (y1L >> b2),
                             (y1L << b1) + x0H, x0L, mod);
      res += s;

      if (borrow)
         y1H += (++y1L == 0);
      borrow = ((x1H < y1H) || (x1H == y1H  &&  x1L < y1L));
      sub_ddmmss(x1H, x1L, x1H, x1L, y1H, y1L);
      sub_ddmmss(y1H, y1L, y0H, y0L, x0H, x0L);
      y1H &= maskH;
      x0L = x1L;
      x0H = x1H & maskH;
   }
}


/*
   Dispatches to one of the above routines depending on b.
*/
void
_nmod_poly_KS2_recover_reduce(mp_ptr res, slong s, mp_srcptr op1,
                          mp_srcptr op2, slong n, ulong b,
                          nmod_t mod)
{
   if (2 * b <= FLINT_BITS)
      _nmod_poly_KS2_recover_reduce1(res, s, op1, op2, n, b, mod);
   else if (b < FLINT_BITS)
      _nmod_poly_KS2_recover_reduce2(res, s, op1, op2, n, b, mod);
   else if (b == FLINT_BITS)
      _nmod_poly_KS2_recover_reduce2b(res, s, op1, op2, n, b, mod);
   else
      _nmod_poly_KS2_recover_reduce3(res, s, op1, op2, n, b, mod);
}
