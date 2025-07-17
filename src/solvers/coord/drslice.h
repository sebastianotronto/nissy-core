/*
TODO

The pruning table for this coordinate shows some odd behavior, which
is actually also present in nissy-classic: after completing depth 14,
if the search is let run for higher depth, no more positions are found.
This could be as simple as a small bug in the logic of the pruning table
calculations - I may be assuming somewhere that I am never going to search
for depths >= 15, since they are the highest possible value anyway -
or some reason related to symmetry - are some cases actually impossible?
In the worst case, it is a bug to be fixed, but I find it unlikely.
*/

#define CLASSES_CP_16 2768

STATIC uint64_t coordinate_cp_coord(cube_t);
STATIC cube_t coordinate_cp_cube(uint64_t);
STATIC uint64_t coordinate_epud_coord(cube_t);
STATIC cube_t coordinate_epud_cube(uint64_t);
STATIC cube_t coordinate_drslice_merge(cube_t, cube_t);

STATIC uint64_t coordinate_drslice_coord(cube_t, const unsigned char *);
STATIC cube_t coordinate_drslice_cube(uint64_t, const unsigned char *);
STATIC bool coordinate_drslice_isnasty(uint64_t, const unsigned char *);
STATIC size_t coordinate_drslice_gendata(unsigned char *);

STATIC bool is_drslice_solvable(cube_t);

STATIC coord_t coordinate_drslice = {
	.name = "DRSLICE",
	.coord = &coordinate_drslice_coord,
	.cube = &coordinate_drslice_cube,
	.isnasty = &coordinate_drslice_isnasty,
	.gendata = coordinate_drslice_gendata,
	.max = CLASSES_CP_16 * FACT_8,
	.trans_mask = TM_UDFIX,
	.moves_mask = MM18_DR,
	.axistrans = {
		[AXIS_UD] = TRANS_UFr,
		[AXIS_RL] = TRANS_RFr,
		[AXIS_FB] = TRANS_FDr,
	},
	.is_admissible = &solution_always_valid,
	.is_solvable = &is_drslice_solvable,
	.pruning_distribution = {
 		[0] = 1,
		[1] = 3,
		[2] = 10,
		[3] = 52,
		[4] = 285,
		[5] = 1318,
		[6] = 5671,
		[7] = 26502,
		[8] = 115467,
		[9] = 470846,
		[10] = 1853056,
		[11] = 6535823,
		[12] = 18349792,
		[13] = 32843350,
		[14] = 34118883,
		[15] = 17284701
	},
	.pruning_max = 15,
	.sym = {
		.classes = CLASSES_CP_16,
		.max = FACT_8,
		.coord = &coordinate_cp_coord,
		.cube = &coordinate_cp_cube,
		.max2 = FACT_8,
		.coord2 = &coordinate_epud_coord,
		.cube2 = &coordinate_epud_cube,
		.merge = &coordinate_drslice_merge,
	},
};

STATIC uint64_t
coordinate_cp_coord(cube_t c)
{
	return coord_cp(c);
}

STATIC cube_t
coordinate_cp_cube(uint64_t i)
{
	return invcoord_cp(i);
}

STATIC uint64_t
coordinate_epud_coord(cube_t c)
{
	return coord_epud(c);
}

STATIC cube_t
coordinate_epud_cube(uint64_t i)
{
	return invcoord_epud(i);
}

STATIC cube_t
coordinate_drslice_merge(cube_t c1, cube_t c2)
{
	cube_t merged;

	merged = c1;
	copy_edges(&merged, c2);

	return merged;
}

STATIC uint64_t
coordinate_drslice_coord(cube_t cube, const unsigned char *data)
{
	return coord_coord_generic(&coordinate_drslice, cube, data);
}

STATIC cube_t
coordinate_drslice_cube(uint64_t i, const unsigned char *data)
{
	return coord_cube_generic(&coordinate_drslice, i, data);
}

STATIC bool
coordinate_drslice_isnasty(uint64_t i, const unsigned char *data)
{
	return coord_isnasty_generic(&coordinate_drslice, i, data);
}

STATIC size_t
coordinate_drslice_gendata(unsigned char *data)
{
	return coord_gendata_generic(&coordinate_drslice, data);
}

STATIC bool
is_drslice_solvable(cube_t cube) {
	return coord_eo(cube) == 0 &&
	       coord_eo(transform_edges(cube, TRANS_URr)) == 0 &&
	       coord_co(cube) == 0;
}
