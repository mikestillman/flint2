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

    Copyright (C) 2011 Sebastian Pancratz

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpq_poly.h"
#include "ulong_extras.h"

int
main(void)
{
    int cflags = 0, i, result;
    flint_rand_t state;
    
    printf("lcm....");
    fflush(stdout);

    flint_randinit(state);

    /* Check aliasing of a and c */
    for (i = 0; i < 100 * flint_test_multiplier(); i++)
    {
        fmpq_poly_t a, b, c;

        fmpq_poly_init(a);
        fmpq_poly_init(b);
        fmpq_poly_init(c);
        fmpq_poly_randtest(a, state, n_randint(state, 100), 100);
        fmpq_poly_randtest(b, state, n_randint(state, 100), 100);

        fmpq_poly_lcm(c, a, b);
        fmpq_poly_lcm(a, a, b);

        cflags |= fmpq_poly_is_canonical(a) ? 0 : 1;
        cflags |= fmpq_poly_is_canonical(c) ? 0 : 2;
        result = (fmpq_poly_equal(a, c) && !cflags);
        if (!result)
        {
            printf("FAIL:\n");
            fmpq_poly_debug(a), printf("\n\n");
            fmpq_poly_debug(b), printf("\n\n");
            fmpq_poly_debug(c), printf("\n\n");
            printf("cflags = %d\n\n", cflags);
            abort();
        }

        fmpq_poly_clear(a);
        fmpq_poly_clear(b);
        fmpq_poly_clear(c);
    }

    /* Check aliasing of b and c */
    for (i = 0; i < 100 * flint_test_multiplier(); i++)
    {
        fmpq_poly_t a, b, c;

        fmpq_poly_init(a);
        fmpq_poly_init(b);
        fmpq_poly_init(c);
        fmpq_poly_randtest(a, state, n_randint(state, 100), 100);
        fmpq_poly_randtest(b, state, n_randint(state, 100), 100);

        fmpq_poly_lcm(c, a, b);
        fmpq_poly_lcm(b, a, b);

        cflags |= fmpq_poly_is_canonical(b) ? 0 : 1;
        cflags |= fmpq_poly_is_canonical(c) ? 0 : 2;
        result = (fmpq_poly_equal(b, c) && !cflags);
        if (!result)
        {
            printf("FAIL:\n");
            fmpq_poly_debug(a), printf("\n\n");
            fmpq_poly_debug(b), printf("\n\n");
            fmpq_poly_debug(c), printf("\n\n");
            printf("cflags = %d\n\n", cflags);
            abort();
        }

        fmpq_poly_clear(a);
        fmpq_poly_clear(b);
        fmpq_poly_clear(c);
    }

    /* Generic case when a, b are most likely co-prime ***********************/

    /* Verify commutativity and that c is monic */
    for (i = 0; i < 100 * flint_test_multiplier(); i++)
    {
        fmpq_poly_t a, b, c;

        fmpq_poly_init(a);
        fmpq_poly_init(b);
        fmpq_poly_init(c);
        fmpq_poly_randtest(a, state, n_randint(state, 100), 100);
        fmpq_poly_randtest(b, state, n_randint(state, 100), 100);

        fmpq_poly_lcm(c, a, b);
        fmpq_poly_lcm(a, b, a);

        cflags |= fmpq_poly_is_canonical(a) ? 0 : 1;
        cflags |= fmpq_poly_is_canonical(c) ? 0 : 2;
        result = (fmpq_poly_equal(a, c) && !cflags 
                  && (fmpq_poly_is_zero(c) || fmpq_poly_is_monic(c)));
        if (!result)
        {
            printf("FAIL:\n");
            fmpq_poly_debug(a), printf("\n\n");
            fmpq_poly_debug(b), printf("\n\n");
            fmpq_poly_debug(c), printf("\n\n");
            printf("cflags = %d\n\n", cflags);
            abort();
        }

        fmpq_poly_clear(a);
        fmpq_poly_clear(b);
        fmpq_poly_clear(c);
    }

    /* Verify that LCM(a, b) GCD(a, b) == a b */
    for (i = 0; i < 100 * flint_test_multiplier(); i++)
    {
        fmpq_poly_t a, b, lcm, gcd;

        fmpq_poly_init(a);
        fmpq_poly_init(b);
        fmpq_poly_init(lcm);
        fmpq_poly_init(gcd);
        fmpq_poly_randtest(a, state, n_randint(state, 100), 100);
        fmpq_poly_randtest(b, state, n_randint(state, 100), 100);

        fmpq_poly_lcm(lcm, a, b);
        fmpq_poly_gcd(gcd, a, b);
        fmpq_poly_mul(lcm, lcm, gcd);
        fmpq_poly_mul(a, a, b);
        fmpq_poly_make_monic(a, a);

        result = fmpq_poly_equal(lcm, a);
        if (!result)
        {
            printf("FAIL:\n");
            fmpq_poly_debug(a), printf("\n\n");
            fmpq_poly_debug(b), printf("\n\n");
            fmpq_poly_debug(lcm), printf("\n\n");
            fmpq_poly_debug(gcd), printf("\n\n");
            abort();
        }

        fmpq_poly_clear(a);
        fmpq_poly_clear(b);
        fmpq_poly_clear(lcm);
        fmpq_poly_clear(gcd);
    }

    /* Case when a, b are not co-prime ***************************************/

    /* Verify commutativity and that c is monic */
    for (i = 0; i < 100 * flint_test_multiplier(); i++)
    {
        fmpq_poly_t a, b, c, t;

        fmpq_poly_init(a);
        fmpq_poly_init(b);
        fmpq_poly_init(c);
        fmpq_poly_init(t);
        fmpq_poly_randtest(a, state, n_randint(state, 100), 100);
        fmpq_poly_randtest(b, state, n_randint(state, 100), 100);
        fmpq_poly_randtest(t, state, n_randint(state, 50), 20);
        fmpq_poly_mul(a, a, t);
        fmpq_poly_mul(b, b, t);

        fmpq_poly_lcm(c, a, b);
        fmpq_poly_lcm(a, b, a);

        cflags |= fmpq_poly_is_canonical(a) ? 0 : 1;
        cflags |= fmpq_poly_is_canonical(c) ? 0 : 2;
        result = (fmpq_poly_equal(a, c) && !cflags 
                  && (fmpq_poly_is_zero(c) || fmpq_poly_is_monic(c)));
        if (!result)
        {
            printf("FAIL:\n");
            fmpq_poly_debug(a), printf("\n\n");
            fmpq_poly_debug(b), printf("\n\n");
            fmpq_poly_debug(c), printf("\n\n");
            printf("cflags = %d\n\n", cflags);
            abort();
        }

        fmpq_poly_clear(a);
        fmpq_poly_clear(b);
        fmpq_poly_clear(c);
        fmpq_poly_clear(t);
    }

    /* Verify that LCM(a, b) GCD(a, b) == a b */
    for (i = 0; i < 100 * flint_test_multiplier(); i++)
    {
        fmpq_poly_t a, b, lcm, gcd, t;

        fmpq_poly_init(a);
        fmpq_poly_init(b);
        fmpq_poly_init(lcm);
        fmpq_poly_init(gcd);
        fmpq_poly_init(t);
        fmpq_poly_randtest(a, state, n_randint(state, 100), 100);
        fmpq_poly_randtest(b, state, n_randint(state, 100), 100);
        fmpq_poly_randtest(t, state, n_randint(state, 50), 20);
        fmpq_poly_mul(a, a, t);
        fmpq_poly_mul(b, b, t);

        fmpq_poly_lcm(lcm, a, b);
        fmpq_poly_gcd(gcd, a, b);
        fmpq_poly_mul(lcm, lcm, gcd);
        fmpq_poly_mul(a, a, b);
        fmpq_poly_make_monic(a, a);

        result = fmpq_poly_equal(lcm, a);
        if (!result)
        {
            printf("FAIL:\n");
            fmpq_poly_debug(a), printf("\n\n");
            fmpq_poly_debug(b), printf("\n\n");
            fmpq_poly_debug(lcm), printf("\n\n");
            fmpq_poly_debug(gcd), printf("\n\n");
            abort();
        }

        fmpq_poly_clear(a);
        fmpq_poly_clear(b);
        fmpq_poly_clear(lcm);
        fmpq_poly_clear(gcd);
        fmpq_poly_clear(t);
    }

    flint_randclear(state);
    flint_cleanup();
    printf("PASS\n");
    return 0;
}


