STATIC_INLINE int64_t coord_cp(cube_t);
STATIC_INLINE cube_t invcoord_cp(int64_t);
STATIC_INLINE int64_t coord_epud(cube_t);
STATIC_INLINE cube_t invcoord_epud(int64_t);

STATIC_INLINE int64_t
coord_cp(cube_t cube)
{
	int i;
	uint8_t c[8], e[12];

	pieces(&cube, c, e);
	for (i = 0; i < 8; i++)
		c[i] &= PBITS;

	return permtoindex(8, c);
}

STATIC_INLINE cube_t
invcoord_cp(int64_t i)
{
	uint8_t c[8];

	indextoperm(i, 8, c);

	return STATIC_CUBE(c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7],
	    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
}

STATIC_INLINE int64_t
coord_epud(cube_t cube)
{
	int i;
	uint8_t c[8], e[12];

	pieces(&cube, c, e);
	DBG_ASSERT(isperm(8, e), -1,
	    "Cannot compute epud coordinate: edges not separated");

	for (i = 0; i < 8; i++)
		e[i] &= PBITS;

	return permtoindex(8, e);
}

STATIC_INLINE cube_t
invcoord_epud(int64_t i)
{
	uint8_t e[8];

	indextoperm(i, 8, e);

	return STATIC_CUBE(0, 1, 2, 3, 4, 5, 6, 7,
	    e[0], e[1], e[2], e[3], e[4], e[5], e[6], e[7], 8, 9, 10, 11);
}
