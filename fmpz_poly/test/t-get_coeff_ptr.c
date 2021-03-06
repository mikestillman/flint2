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

int
main(void)
{
    int i, result;
    flint_rand_t state;

    printf("get_coeff_ptr....");
    fflush(stdout);

    flint_randinit(state);

    for (i = 0; i < 1000 * flint_test_multiplier(); i++)
    {
        fmpz_poly_t A;
        fmpz_t a;
        slong n = n_randint(state, 100);

        fmpz_poly_init(A);
        fmpz_poly_randtest(A, state, n_randint(state, 100), 100);
        fmpz_init(a);

        fmpz_poly_get_coeff_fmpz(a, A, n);

        result = n < fmpz_poly_length(A) ? 
                     fmpz_equal(a, fmpz_poly_get_coeff_ptr(A, n)) : 
                     fmpz_poly_get_coeff_ptr(A, n) == NULL;
        if (!result)
        {
            printf("FAIL:\n");
            printf("A = "), fmpz_poly_print(A), printf("\n\n");
            printf("a = "), fmpz_print(a), printf("\n\n");
            printf("n = %ld\n\n", n);
            abort();
        }

        fmpz_poly_clear(A);
        fmpz_clear(a);
    }

    flint_randclear(state);
    flint_cleanup();
    printf("PASS\n");
    return 0;
}
