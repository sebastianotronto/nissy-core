STATIC cube_t coordinate_cpepe_merge(const cube_t, const cube_t);
STATIC uint64_t coordinate_cpepe_coord(const cube_t, const unsigned char *);
STATIC cube_t coordinate_cpepe_cube(uint64_t, const unsigned char *);
STATIC bool coordinate_cpepe_isnasty(uint64_t, const unsigned char *);
STATIC size_t coordinate_cpepe_gendata(unsigned char *);

STATIC coord_t coordinate_cpepe = {
	.name = "CPEPE",
	.coord = &coordinate_cpepe_coord,
	.cube = &coordinate_cpepe_cube,
	.isnasty = &coordinate_cpepe_isnasty,
	.gendata = coordinate_cpepe_gendata,
	.max = CLASSES_CP_16 * FACT_4,
	.trans_mask = TM_UDFIX,
	.moves_mask_gendata = MM18_DR,
	.moves_mask_solve = MM18_DR,
	.is_admissible = &solution_always_valid,
	.is_solvable = &is_drfin_solvable,
	.is_solved = NULL,
	.allow_niss = false,
	.pruning_distribution = {
		/* TODO */
 		[0] = 0,
		[1] = 0,
		[2] = 0,
		[3] = 0,
		[4] = 0,
		[5] = 0,
		[6] = 0,
		[7] = 0,
		[8] = 0,
		[9] = 0,
		[10] = 0,
		[11] = 0,
		[12] = 0,
		[13] = 0,
		[14] = 0,
		[15] = 0,
	},
	.pruning_max = 15, /* TODO */
	.sym = {
		.classes = CLASSES_CP_16,
		.max = FACT_8,
		.coord = &coord_cp,
		.cube = &invcoord_cp,
		.max2 = FACT_4,
		.coord2 = &coord_epe,
		.cube2 = &invcoord_epe,
		.merge = &coordinate_cpepe_merge,
	},
};

STATIC cube_t
coordinate_cpepe_merge(const cube_t c1, const cube_t c2)
{
	cube_t merged;

	merged = c1;
	copy_edges(&merged, c2);

	return merged;
}

STATIC uint64_t
coordinate_cpepe_coord(const cube_t cube, const unsigned char *data)
{
	return coord_coord_generic(&coordinate_cpepe, cube, data);
}

STATIC cube_t
coordinate_cpepe_cube(uint64_t i, const unsigned char *data)
{
	return coord_cube_generic(&coordinate_cpepe, i, data);
}

STATIC bool
coordinate_cpepe_isnasty(uint64_t i, const unsigned char *data)
{
	return coord_isnasty_generic(&coordinate_cpepe, i, data);
}

STATIC size_t
coordinate_cpepe_gendata(unsigned char *data)
{
	return coord_gendata_generic(&coordinate_cpepe, data);
}
