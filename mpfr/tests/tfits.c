/* Test file for:
 mpfr_fits_sint_p, mpfr_fits_slong_p, mpfr_fits_sshort_p,
 mpfr_fits_uint_p, mpfr_fits_ulong_p, mpfr_fits_ushort_p

Copyright 2004 Free Software Foundation, Inc.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "mpfr-test.h"

#define ERROR1 { printf("Initial error for x="); mpfr_dump(x); exit(1); }
#define ERROR2 { printf("Error for x="); mpfr_dump(x); exit(1); }

int
main (void)
{
  mpfr_t x;

  tests_start_mpfr ();

  mpfr_init2 (x, 256);

  /* Check NAN */
  mpfr_set_nan(x);
  if (mpfr_fits_ulong_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_slong_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_uint_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_sint_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_ushort_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_sshort_p(x, GMP_RNDN))
    ERROR1;

  /* Check INF */
  mpfr_set_inf(x, 1);
  if (mpfr_fits_ulong_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_slong_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_uint_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_sint_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_ushort_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_sshort_p(x, GMP_RNDN))
    ERROR1;

  /* Check Zero */
  MPFR_SET_ZERO(x);
  if (!mpfr_fits_ulong_p(x, GMP_RNDN))
    ERROR2;
  if (!mpfr_fits_slong_p(x, GMP_RNDN))
    ERROR2;
  if (!mpfr_fits_uint_p(x, GMP_RNDN))
    ERROR2;
  if (!mpfr_fits_sint_p(x, GMP_RNDN))
    ERROR2;
  if (!mpfr_fits_ushort_p(x, GMP_RNDN))
    ERROR2;
  if (!mpfr_fits_sshort_p(x, GMP_RNDN))
    ERROR2;

  /* Check small op */
  mpfr_set_str1 (x, "1@-1");
  if (!mpfr_fits_ulong_p(x, GMP_RNDN))
    ERROR2;
  if (!mpfr_fits_slong_p(x, GMP_RNDN))
    ERROR2;
  if (!mpfr_fits_uint_p(x, GMP_RNDN))
    ERROR2;
  if (!mpfr_fits_sint_p(x, GMP_RNDN))
    ERROR2;
  if (!mpfr_fits_ushort_p(x, GMP_RNDN))
    ERROR2;
  if (!mpfr_fits_sshort_p(x, GMP_RNDN))
    ERROR2;

  /* Check all other values */
  mpfr_set_ui(x, ULONG_MAX, GMP_RNDN);
  mpfr_mul_2exp(x, x, 1, GMP_RNDN);
  if (mpfr_fits_ulong_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_slong_p(x, GMP_RNDN))
    ERROR1;
  mpfr_mul_2exp(x, x, 40, GMP_RNDN);
  if (mpfr_fits_ulong_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_uint_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_sint_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_ushort_p(x, GMP_RNDN))
    ERROR1;
  if (mpfr_fits_sshort_p(x, GMP_RNDN))
    ERROR1;

  mpfr_set_ui(x, ULONG_MAX, GMP_RNDN);
  if (!mpfr_fits_ulong_p(x, GMP_RNDN))
    ERROR2;
  mpfr_set_ui(x, LONG_MAX, GMP_RNDN);
  if (!mpfr_fits_slong_p(x, GMP_RNDN))
    ERROR2;
  mpfr_set_ui(x, UINT_MAX, GMP_RNDN);
  if (!mpfr_fits_uint_p(x, GMP_RNDN))
    ERROR2;
  mpfr_set_ui(x, INT_MAX, GMP_RNDN);
  if (!mpfr_fits_sint_p(x, GMP_RNDN))
    ERROR2;
  mpfr_set_ui(x, USHRT_MAX, GMP_RNDN);
  if (!mpfr_fits_ushort_p(x, GMP_RNDN))
    ERROR2;
  mpfr_set_ui(x, SHRT_MAX, GMP_RNDN);
  if (!mpfr_fits_sshort_p(x, GMP_RNDN))
    ERROR2;

  mpfr_set_si(x, 1, GMP_RNDN);
  if (!mpfr_fits_sint_p(x, GMP_RNDN))
    ERROR2;
  if (!mpfr_fits_sshort_p(x, GMP_RNDN))
    ERROR2;

  mpfr_clear (x);

  tests_end_mpfr ();
  return 0;
}