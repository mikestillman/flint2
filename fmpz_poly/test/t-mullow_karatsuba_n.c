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

    Copyright (C) 2009 William Hart

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpz_poly.h"
#include "ulong_extras.h"

int
main(void)
{
    int i, result;
    flint_rand_t state;


    printf("mullow_karatsuba_n....");
    fflush(stdout);

    flint_randinit(state);

    /* Check aliasing of a and b */
    for (i = 0; i < 200 * flint_test_multiplier(); i++)
    {
        fmpz_poly_t a, b, c;
        slong len;

        fmpz_poly_init(a);
        fmpz_poly_init(b);
        fmpz_poly_init(c);
        len = n_randint(state, 50);
        fmpz_poly_randtest(b, state, len, 200);
        fmpz_poly_randtest(c, state, len, 200);

        fmpz_poly_mullow_karatsuba_n(a, b, c, len);
        fmpz_poly_mullow_karatsuba_n(b, b, c, len);

        result = (fmpz_poly_equal(a, b));
        if (!result)
        {
            printf("FAIL:\n");
            fmpz_poly_print(a), printf("\n\n");
            fmpz_poly_print(b), printf("\n\n");
            abort();
        }

        fmpz_poly_clear(a);
        fmpz_poly_clear(b);
        fmpz_poly_clear(c);
    }

    /* Check aliasing of a and c */
    for (i = 0; i < 200 * flint_test_multiplier(); i++)
    {
        fmpz_poly_t a, b, c;
        slong len;

        fmpz_poly_init(a);
        fmpz_poly_init(b);
        fmpz_poly_init(c);
        len = n_randint(state, 50);
        fmpz_poly_randtest(b, state, len, 200);
        fmpz_poly_randtest(c, state, len, 200);

        fmpz_poly_mullow_karatsuba_n(a, b, c, len);
        fmpz_poly_mullow_karatsuba_n(c, b, c, len);

        result = (fmpz_poly_equal(a, c));
        if (!result)
        {
            printf("FAIL:\n");
            fmpz_poly_print(a), printf("\n\n");
            fmpz_poly_print(c), printf("\n\n");
            abort();
        }

        fmpz_poly_clear(a);
        fmpz_poly_clear(b);
        fmpz_poly_clear(c);
    }

    /* Compare with mul_karatsuba */
    for (i = 0; i < 200 * flint_test_multiplier(); i++)
    {
        fmpz_poly_t a, b, c, d;
        slong len;

        fmpz_poly_init(a);
        fmpz_poly_init(b);
        fmpz_poly_init(c);
        fmpz_poly_init(d);
        len = n_randint(state, 50);
        fmpz_poly_randtest(b, state, len, 200);
        fmpz_poly_randtest(c, state, len, 200);

        fmpz_poly_mullow_karatsuba_n(a, b, c, len);
        fmpz_poly_mul_karatsuba(d, b, c);
        fmpz_poly_truncate(d, len);

        result = (fmpz_poly_equal(a, d));
        if (!result)
        {
            printf("FAIL:\n");
            fmpz_poly_print(a), printf("\n\n");
            fmpz_poly_print(d), printf("\n\n");
            abort();
        }

        fmpz_poly_clear(a);
        fmpz_poly_clear(b);
        fmpz_poly_clear(c);
        fmpz_poly_clear(d);
    }

    flint_randclear(state);
    flint_cleanup();
    printf("PASS\n");
    return 0;
}
