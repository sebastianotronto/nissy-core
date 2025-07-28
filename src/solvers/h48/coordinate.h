STATIC_INLINE int64_t coord_h48(
    cube_t, const uint32_t [static COCSEP_TABLESIZE], uint8_t);
STATIC_INLINE int64_t coord_h48_edges(cube_t, int64_t, uint8_t, uint8_t);
STATIC_INLINE cube_t invcoord_h48(
    int64_t, const cube_t [static COCSEP_CLASSES], uint8_t);

STATIC_INLINE int64_t
coord_h48(
	cube_t c,
	const uint32_t cocsepdata[static COCSEP_TABLESIZE],
	uint8_t h
)
{
	int64_t cocsep, coclass;
	uint32_t data;
	uint8_t ttrep;

	DBG_ASSERT(h <= 11, "coord_h48: h must be between 0 and 11\n");

	cocsep = coord_cocsep(c);
	data = cocsepdata[cocsep];
	coclass = (int64_t)COCLASS(data);
	ttrep = (int64_t)TTREP(data);

	return coord_h48_edges(c, coclass, ttrep, h);
}

STATIC_INLINE int64_t
coord_h48_edges(cube_t c, int64_t coclass, uint8_t ttrep, uint8_t h)
{
	cube_t d;
	int64_t esep, eo, edges;

	d = transform_edges(c, ttrep);
	esep = coord_esep(d);
	eo = coord_eo(d);
	edges = (esep << 11) + eo;

	return (coclass * H48_ESIZE(11) + edges) >> (11 - (int64_t)h);
}

/*
This function does not necessarily return a cube whose coordinate is
the given value, because it works up to symmetry. This means that the
returned cube is a transformed cube of one that gives the correct value.
*/
STATIC_INLINE cube_t
invcoord_h48(
	int64_t i,
	const cube_t crep[static COCSEP_CLASSES],
	uint8_t h
)
{
	cube_t ret;
	int64_t hh, coclass, ee, esep, eo;

	DBG_ASSERT(h <= 11, "invcoord_h48: h must be between 0 and 11\n");

	hh = (int64_t)h;
	coclass = i / H48_ESIZE(h);
	ee = i % H48_ESIZE(h);
	esep = ee >> hh;
	eo = (ee & ((1 << hh) - 1)) << (11 - hh);

	ret = invcoord_esep(esep);
	copy_corners(&ret, crep[coclass]);
	set_eo(&ret, eo);

	return ret;
}
