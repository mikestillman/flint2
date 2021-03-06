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

    Copyright (C) 2010 Sebastian Pancratz

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

    printf("evaluate_divconquer_fmpz....");
    fflush(stdout);

    flint_randinit(state);

    /* Check aliasing */
    for (i = 0; i < 1000 * flint_test_multiplier(); i++)
    {
        fmpz_t a, b;
        fmpz_poly_t f;

        fmpz_init(a);
        fmpz_init(b);
        fmpz_poly_init(f);
        fmpz_poly_randtest(f, state, n_randint(state, 100), 200);
        fmpz_randtest(a, state, 100);

        fmpz_poly_evaluate_divconquer_fmpz(b, f, a);
        fmpz_poly_evaluate_divconquer_fmpz(a, f, a);

        result = (fmpz_equal(a, b));
        if (!result)
        {
            printf("FAIL (alias):\n");
            fmpz_print(a), printf("\n\n");
            fmpz_print(b), printf("\n\n");
            abort();
        }

        fmpz_clear(a);
        fmpz_clear(b);
        fmpz_poly_clear(f);
    }

    /* Check that the result agrees with Horner's method */
    for (i = 0; i < 1000 * flint_test_multiplier(); i++)
    {
        fmpz_t a, b, c;
        fmpz_poly_t f;

        fmpz_init(a);
        fmpz_init(b);
        fmpz_init(c);
        fmpz_poly_init(f);
        fmpz_poly_randtest(f, state, n_randint(state, 100), 200);
        fmpz_randtest(a, state, 100);

        fmpz_poly_evaluate_divconquer_fmpz(b, f, a);
        fmpz_poly_evaluate_horner_fmpz(c, f, a);

        result = (fmpz_equal(b, c));
        if (!result)
        {
            printf("FAIL (cmp with Horner):\n");
            fmpz_print(a), printf("\n\n");
            fmpz_print(b), printf("\n\n");
            fmpz_print(c), printf("\n\n");
            abort();
        }

        fmpz_clear(a);
        fmpz_clear(b);
        fmpz_clear(c);
        fmpz_poly_clear(f);
    }

    flint_randclear(state);
    flint_cleanup();
    printf("PASS\n");
    return 0;
}
