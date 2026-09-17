#ifndef SRC_SOLVERS_TWOSTEP_TWOSTEP_H
#define SRC_SOLVERS_TWOSTEP_TWOSTEP_H

#include "../coord/coord.h"

STATIC size_t twostep_size_1(void);
STATIC size_t twostep_size_2(void);
STATIC long long dataid_twostep(const char *, char [SIZE(NISSY_SIZE_DATAID)]);
STATIC long long gendata_twostep(
    const char *, unsigned long long, unsigned char *);
STATIC long long checkdata_twostep(
    const char *, unsigned long long, const unsigned char *);
STATIC long long solve_twostep(oriented_cube_t, const char *, unsigned,
    unsigned, unsigned, unsigned, unsigned, unsigned, unsigned long long,
    const unsigned char *, unsigned, char *,
    long long [SIZE(NISSY_SIZE_SOLVE_STATS)], int (*)(void *), void *);

STATIC size_t
twostep_size_1(void)
{
	size_t s;

	s = gendata_coord(&coordinate_dr, NULL);
	s = (size_t)8 * DIV_ROUND_UP(s, 8); /* Preserve 8-byte alignment */

	return s;
}

STATIC size_t
twostep_size_2(void)
{
	return gendata_multicoord(&multicoordinate_drfin, NULL);
}

STATIC long long
dataid_twostep(const char *s, char dataid[SIZE(NISSY_SIZE_DATAID)])
{
	strcpy(dataid, "DR_DRFIN");
	return NISSY_OK;
}

STATIC long long
gendata_twostep(
	const char *s,
	unsigned long long bufsize,
	unsigned char *buf
)
{
	size_t r, s1, s2;

	s1 = twostep_size_1();
	s2 = twostep_size_2();

	if (buf == NULL)
		goto gendata_twostep_return_size;

	if (bufsize < s1 + s2)
		return NISSY_ERROR_BUFFER_SIZE;

	r = gendata_coord(&coordinate_dr, buf);
	if (r == 0)
		return NISSY_ERROR_UNKNOWN;

	r = gendata_multicoord(&multicoordinate_drfin, buf + s1);
	if (r == 0)
		return NISSY_ERROR_UNKNOWN;

gendata_twostep_return_size:
	return s1 + s2;
}

STATIC long long
checkdata_twostep(
	const char *s,
	unsigned long long bufsize,
	const unsigned char *buf
)
{
	long long r;

	LOG("[checkdata] Checking double table for two step solver\n");

	r = checkdata_coord(&coordinate_dr, bufsize, buf);
	if (r != NISSY_OK)
		return r;

	r = checkdata_multicoord(
	    &multicoordinate_drfin, bufsize, buf + twostep_size_1());
	if (r != NISSY_OK)
		return r;

	return NISSY_OK;
}

STATIC long long
solve_twostep(
	oriented_cube_t oc,
	const char *coord_and_trans,
	unsigned nissflag,
	unsigned minmoves,
	unsigned maxmoves,
	unsigned maxsolutions,
	unsigned optimal,
	unsigned threads,
	unsigned long long data_size,
	const unsigned char *data,
	unsigned solutions_size,
	char *sols,
	long long stats[SIZE(NISSY_SIZE_SOLVE_STATS)],
	int (*poll_status)(void *),
	void *poll_status_data
)
{
//TODO
}

#endif /* SRC_SOLVERS_TWOSTEP_TWOSTEP_H */
