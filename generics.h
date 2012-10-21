/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2012 Fredrik Johansson

******************************************************************************/

#ifndef GENERICS_H
#define GENERICS_H

#include "flint.h"
#include "fmpz.h"
#include "fmpz_poly.h"
#include "fmpz_vec.h"
#include "nmod_vec.h"
#include "ulong_extras.h"

#ifdef __cplusplus
 extern "C" {
#endif

struct ring_struct;
struct elem_struct;

typedef enum
{
    TYPE_FMPZ,
    TYPE_LIMB,
    TYPE_MOD,
    TYPE_FRAC,
    TYPE_POLY
}
ring_type;

/* todo: make this a union */
typedef struct
{
    ring_type type;
    long size;

    void * parent;

    nmod_t nmod;
    void * modulus;

    void * numer;
    void * denom;
    long denom_offset;
}
ring_struct;

typedef struct
{
    void * coeffs;
    long length;
    long alloc;
} elem_poly_struct;

typedef void * elem_ptr;
typedef const void * elem_srcptr;

#define INDEX(_ptr, _i, _size) ((char *)(_ptr) + ((_i) * (_size)))
#define SRC_INDEX(_ptr, _i, _size) ((const char *)(_ptr) + ((_i) * (_size)))

typedef struct
{
    ring_struct * ring;
    elem_ptr elem;
} gen_struct;


typedef ring_struct ring_t[1];
typedef gen_struct gen_t[1];

#define RING_PARENT(ring) ((ring_struct *) ((ring)->parent))
#define RING_MODULUS(ring) ((elem_ptr) ((ring)->modulus))

#define RING_NUMER(ring) ((ring_struct *) ((ring)->numer))
#define RING_DENOM(ring) ((ring_struct *) ((ring)->denom))

#define NUMER(elem, ring) ((elem_ptr) (elem))
#define DENOM(elem, ring) ((elem_ptr) (((char *) (elem)) + (ring)->denom_offset))

#define NOT_IMPLEMENTED(opname, ring) do { \
    printf("operation %s not implemented for ring ", opname); \
    ring_print(ring); printf("\n"); \
    abort(); \
  } while (0)

static __inline__ void
gen_swap(gen_t res, gen_t src)
{
    gen_struct t = *src;
    *src = *res;
    *res = t;
}

static __inline__ void
elem_poly_swap(elem_poly_struct * op1, elem_poly_struct * op2)
{
    elem_poly_struct t = *op1;
    *op1 = *op2;
    *op2 = t;
}

void ring_init_fmpz(ring_t ring);
void ring_init_limb(ring_t ring);
void ring_init_mod(ring_t ring, const ring_t elem_ring, elem_srcptr modulus);
void ring_init_frac(ring_t ring, const ring_t numer_ring, const ring_t denom_ring);
void ring_init_poly(ring_t ring, const ring_t elem_ring);
void ring_clear(ring_t ring);
void ring_print(const ring_t ring);

void elem_init(elem_ptr elem, const ring_t ring);
void gen_init(gen_t x, const ring_t ring);

void elem_clear(elem_ptr elem, const ring_t ring);
void gen_clear(gen_t x);

void elem_zero(elem_ptr x, const ring_t ring);
void gen_zero(gen_t x);

int elem_is_zero(elem_srcptr x, const ring_t ring);
int gen_is_zero(const gen_t x);

void elem_one(elem_ptr x, const ring_t ring);
void gen_one(gen_t x);

int elem_is_one(elem_srcptr x, const ring_t ring);
int gen_is_one(const gen_t x);

int elem_equal(elem_srcptr op1, elem_srcptr op2, const ring_t ring);
int gen_equal(const gen_t op1, const gen_t op2);

void _elem_poly_fit_length(elem_poly_struct * poly, long len, const ring_t poly_ring);
void _elem_poly_set_length(elem_poly_struct * poly, long len, const ring_t poly_ring);
void _elem_poly_normalise(elem_poly_struct * poly, const ring_t poly_ring);

void _elem_poly_print(elem_srcptr poly, long len, const ring_t ring);
void elem_print(elem_srcptr elem, const ring_t ring);
void gen_print(gen_t x);

void _elem_vec_set(elem_ptr res, elem_srcptr src, long len, const ring_t ring);
void elem_set(elem_ptr res, elem_srcptr src, const ring_t ring);
void gen_set(gen_t y, const gen_t x);

void elem_set_si(elem_ptr elem, long v, const ring_t ring);
void gen_set_si(gen_t x, long v);

void elem_set_coeff_si(elem_poly_struct * elem, long index, long value, const ring_t ring);
void gen_set_coeff_si(gen_t x, long index, long value);

void elem_randtest(elem_ptr res, flint_rand_t state, const long * size, const ring_t ring);
void gen_randtest(gen_t res, flint_rand_t state, const long * size);

void _elem_vec_neg(elem_ptr res, elem_srcptr src, long len, const ring_t ring);
void gen_neg(gen_t y, const gen_t x);
void elem_neg(elem_ptr res, elem_srcptr src, const ring_t ring);

void _elem_poly_add(elem_ptr res, elem_srcptr poly1, long len1, elem_srcptr poly2, long len2, const ring_t ring);
void elem_add(elem_ptr res, elem_srcptr op1, elem_srcptr op2, const ring_t ring);
void gen_add(gen_t z, const gen_t x, const gen_t y);

void _elem_poly_sub(elem_ptr res, elem_srcptr poly1, long len1, elem_srcptr poly2, long len2, const ring_t ring);
void elem_sub(elem_ptr res, elem_srcptr op1, elem_srcptr op2, const ring_t ring);
void gen_sub(gen_t z, const gen_t x, const gen_t y);

void elem_mul(elem_ptr res, elem_srcptr op1, elem_srcptr op2, const ring_t ring);
void gen_mul(gen_t z, const gen_t x, const gen_t y);

void _elem_poly_mul(elem_ptr res, elem_srcptr poly1, long len1, elem_srcptr poly2, long len2, const ring_t ring);


void elem_poly_divrem(elem_poly_struct * Q, elem_poly_struct * R,
                      const elem_poly_struct * A, const elem_poly_struct * B, const ring_t ring);
void elem_divrem(elem_ptr Q, elem_ptr R, elem_srcptr A, elem_srcptr B, const ring_t ring);
void gen_divrem(gen_t Q, gen_t R, const gen_t A, const gen_t B);

void elem_divexact(elem_ptr Q, elem_srcptr A, elem_srcptr B, const ring_t ring);

void _elem_vec_scalar_mul(elem_ptr res, elem_srcptr vec, long len, elem_srcptr c, const ring_t ring);
void _elem_vec_scalar_addmul(elem_ptr res, elem_srcptr vec, long len, elem_srcptr c, const ring_t ring);

void elem_content_recursive(elem_ptr cont, elem_srcptr obj, const ring_t cont_ring, const ring_t obj_ring);
void elem_div_content_recursive(elem_ptr obj, elem_srcptr cont, const ring_t cont_ring, const ring_t obj_ring);

void elem_frac_canonicalise(elem_srcptr x, const ring_t ring);

#ifdef __cplusplus
}
#endif

#endif

