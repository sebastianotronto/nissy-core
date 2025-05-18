typedef struct {
	cube_t cube;
	cube_t inverse;
	uint8_t target_depth;
	solution_moves_t *solution_moves;
	solution_settings_t *solution_settings;
	solution_list_t *solution_list;
	uint8_t nissflag;
	bool lastisnormal;
	coord_t *coord;
	const unsigned char *coord_data;
	const unsigned char *ptable;
} dfsarg_solve_coord_t;

STATIC int64_t solve_coord(oriented_cube_t, coord_t [static 1], uint8_t,
    uint8_t, uint8_t, uint8_t, uint64_t, uint8_t, uint8_t, uint64_t n,
    const unsigned char [n], size_t m, char [m], int (*)(void *), void *);
STATIC long long solve_coord_dispatch(oriented_cube_t, const char *, unsigned,
    unsigned, unsigned, unsigned, unsigned, unsigned, unsigned long long n,
    const unsigned char [n], unsigned m, char [m],
    long long [static NISSY_SIZE_SOLVE_STATS],
    int (*)(void *), void *);
STATIC bool coord_solution_admissible(const dfsarg_solve_coord_t [static 1]);
STATIC bool solve_coord_dfs_stop(const dfsarg_solve_coord_t [static 1]);
STATIC bool coord_continue_onnormal(const dfsarg_solve_coord_t [static 1]);
STATIC bool coord_continue_oninverse(const dfsarg_solve_coord_t [static 1]);
STATIC int64_t solve_coord_dfs(dfsarg_solve_coord_t [static 1]);

STATIC bool
coord_solution_admissible(const dfsarg_solve_coord_t arg[static 1])
{
	uint8_t n;

	n = arg->solution_moves->nmoves + arg->solution_moves->npremoves;
	if (arg->target_depth != n)
		return false;

	return arg->coord->is_admissible == NULL ||
	    arg->coord->is_admissible(arg->solution_moves);
}

STATIC bool
solve_coord_dfs_stop(const dfsarg_solve_coord_t arg[static 1])
{
	bool hasnissed;
	uint8_t n, pval;
	uint64_t coord;
	const cube_t *c;

	n = arg->solution_moves->nmoves + arg->solution_moves->npremoves;
	if (n >= arg->target_depth)
		return true;

	hasnissed = arg->solution_moves->nmoves > 0 &&
	    arg->solution_moves->npremoves > 0;
	if (!hasnissed && (arg->nissflag & NISSY_NISSFLAG_MIXED))
		return false;

	c = arg->lastisnormal ? &arg->cube : &arg->inverse;

	coord = arg->coord->coord(*c, arg->coord_data);
	pval = get_coord_pval(arg->coord, arg->ptable, coord);

	return n + pval > arg->target_depth;
}

STATIC bool
coord_continue_onnormal(const dfsarg_solve_coord_t arg[static 1])
{
	uint8_t f, nn, ni, th;

	f = arg->nissflag;
	nn = arg->solution_moves->nmoves;
	ni = arg->solution_moves->npremoves;
	th = DIV_ROUND_UP(arg->target_depth, 2);

	if (nn + ni == 0)
		return f & (NISSY_NISSFLAG_NORMAL | NISSY_NISSFLAG_MIXED);

	if (arg->lastisnormal)
		return (f & NISSY_NISSFLAG_NORMAL) ||
		    ((f & NISSY_NISSFLAG_MIXED) && (ni > 0 || nn <= th));

	return (f & NISSY_NISSFLAG_MIXED) && nn == 0 && ni < th &&
	    coord_can_switch(arg->coord, arg->coord_data,
	        ni, arg->solution_moves->premoves);
}

STATIC bool
coord_continue_oninverse(const dfsarg_solve_coord_t arg[static 1])
{
	uint8_t f, nn, ni, th;

	f = arg->nissflag;
	nn = arg->solution_moves->nmoves;
	ni = arg->solution_moves->npremoves;
	th = DIV_ROUND_UP(arg->target_depth, 2);

	if (nn + ni == 0)
		return f & (NISSY_NISSFLAG_INVERSE | NISSY_NISSFLAG_MIXED);

	if (!arg->lastisnormal)
		return (f & NISSY_NISSFLAG_INVERSE) ||
		    ((f & NISSY_NISSFLAG_MIXED) && (nn > 0 || ni < th));

	return (f & NISSY_NISSFLAG_MIXED) && ni == 0 && nn <= th &&
	    coord_can_switch(arg->coord, arg->coord_data,
	        nn, arg->solution_moves->moves);
}

STATIC int64_t
solve_coord_dfs(dfsarg_solve_coord_t arg[static 1])
{
	bool lastbackup;
	uint8_t m, l, nnbackup, nibackup;
	uint32_t mm;
	uint64_t coord;
	int64_t n, ret;
	cube_t backup_cube, backup_inverse;

	coord = arg->coord->coord(arg->cube, arg->coord_data);
	if (coord == 0) {
		if (!coord_solution_admissible(arg))
			return 0;
		return appendsolution(arg->solution_moves,
		    arg->solution_settings, arg->solution_list, true,
		    arg->coord->name);
	}

	if (solve_coord_dfs_stop(arg))
		return 0;

	backup_cube = arg->cube;
	backup_inverse = arg->inverse;
	lastbackup = arg->lastisnormal;
	nnbackup = arg->solution_moves->nmoves;
	nibackup = arg->solution_moves->npremoves;

	ret = 0;
	if (coord_continue_onnormal(arg)) {
		l = arg->solution_moves->nmoves;
		mm = arg->coord->moves_mask;
		if (l != 0) {
			m = arg->solution_moves->moves[l-1];
			mm &= allowedmask[movebase(m)];
		}
		arg->solution_moves->nmoves++;
		arg->lastisnormal = true;

		for (m = 0; m < NMOVES; m++) {
			if (!(mm & (UINT32_C(1) << (uint32_t)m)))
				continue;

			arg->solution_moves->moves[l] = m;
			arg->cube = move(backup_cube, m);
			arg->inverse = premove(backup_inverse, m);
			n = solve_coord_dfs(arg);
			if (n < 0)
				return n;
			ret += n;
			arg->solution_moves->npremoves = nibackup;
		}

		arg->solution_moves->nmoves--;
	}

	arg->lastisnormal = lastbackup;

	if (coord_continue_oninverse(arg)) {
		l = arg->solution_moves->npremoves;
		mm = arg->coord->moves_mask;
		if (l != 0) {
			m = arg->solution_moves->premoves[l-1];
			mm &= allowedmask[movebase(m)];
		}
		arg->solution_moves->npremoves++;
		arg->lastisnormal = false;
		
		for (m = 0; m < NMOVES; m++) {
			if (!(mm & (UINT32_C(1) << (uint32_t)m)))
				continue;

			arg->solution_moves->premoves[l] = m;
			arg->inverse = move(backup_inverse, m);
			arg->cube = premove(backup_cube, m);
			n = solve_coord_dfs(arg);
			if (n < 0)
				return n;
			ret += n;
			arg->solution_moves->nmoves = nnbackup;
		}

		arg->solution_moves->npremoves--;
	}

	arg->cube = backup_cube;
	arg->inverse = backup_inverse;
	arg->lastisnormal = lastbackup;

	return ret;
}

STATIC long long
solve_coord_dispatch(
	oriented_cube_t oc,
	const char *coord_and_axis,
	unsigned nissflag,
	unsigned minmoves,
	unsigned maxmoves,
	unsigned maxsolutions,
	unsigned optimal,
	unsigned threads,
	unsigned long long data_size,
	const unsigned char data[data_size],
	unsigned solutions_size,
	char sols[solutions_size],
	long long stats[static NISSY_SIZE_SOLVE_STATS],
	int (*poll_status)(void *),
	void *poll_status_data
)
{
	coord_t *coord;
	uint8_t axis;

	parse_coord_and_axis(
	    strlen(coord_and_axis), coord_and_axis, &coord, &axis);

	if (coord == NULL) {
		LOG("Error: could not parse coordinate from '%s'\n",
		    coord_and_axis);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	if (axis == UINT8_ERROR) {
		LOG("Error: could not parse axis from '%s'\n", coord_and_axis);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	return solve_coord(oc, coord, axis, nissflag, minmoves, maxmoves,
	    maxsolutions, optimal, threads, data_size, data,
	    solutions_size, sols, poll_status, poll_status_data);
}

STATIC int64_t
solve_coord(
	oriented_cube_t oc,
	coord_t coord [static 1],
	uint8_t axis,
	uint8_t nissflag,
	uint8_t minmoves,
	uint8_t maxmoves,
	uint64_t maxsolutions,
	uint8_t optimal,
	uint8_t threads,
	uint64_t data_size,
	const unsigned char data[data_size],
	size_t solutions_size,
	char sols[solutions_size],
	int (*poll_status)(void *),
	void *poll_status_data
)
{
	int8_t d;
	uint8_t t;
	int64_t ndepth;
	cube_t c;
	const unsigned char *coord_data;
	const unsigned char *ptable;
	dfsarg_solve_coord_t arg;
	tableinfo_t info;
	solution_moves_t solution_moves;
	solution_settings_t solution_settings;
	solution_list_t solution_list;

	t = coord->axistrans[axis];
	c = transform(oc.cube, t);

	if (!coord->is_solvable(c))
		goto solve_coord_error_unsolvable;

	if (!solution_list_init(&solution_list, solutions_size, sols))
		goto solve_coord_error_buffer;

	if (readtableinfo(data_size, data, &info) != NISSY_OK)
		goto solve_coord_error_data;

	if (info.type == TABLETYPE_PRUNING) {
		/* Only the pruning table */
		coord_data = NULL;
		ptable = data + INFOSIZE;
	} else {
		/* Coordinate has extra data */
		coord_data = data + INFOSIZE;
		ptable = data + info.next + INFOSIZE;
	}

	solution_moves_reset(&solution_moves);

	solution_settings = (solution_settings_t) {
		.tmask = TM_SINGLE(inverse_trans(t)),
		.unniss = false,
		.maxmoves = maxmoves,
		.maxsolutions = maxsolutions,
		.optimal = optimal,
		.orientation = oc.orientation,
	};

	arg = (dfsarg_solve_coord_t) {
		.cube = c,
		.inverse = inverse(c),
		.coord = coord,
		.coord_data = coord_data,
		.ptable = ptable,
		.solution_moves = &solution_moves,
		.solution_settings = &solution_settings,
		.solution_list = &solution_list,
		.nissflag = nissflag,

		/*
		Since no move has been done yet, this field should be
		neither true nor false; using its value now is logically
		undefined behavior.
		TODO: find a more elegant solution
		*/
		.lastisnormal = true,
	};

	if (coord->coord(c, coord_data) == 0) {
		if (minmoves == 0 && !appendsolution(&solution_moves,
		    &solution_settings, &solution_list, true, coord->name))
				goto solve_coord_error_buffer;
		goto solve_coord_done;
	}

	for (
	    d = MAX(minmoves, 1);
	    !solutions_done(&solution_list, &solution_settings, d);
	    d++
	) {
		if (d >= 12)
			LOG("[%s solve] Found %" PRIu64 " solutions, "
			    "searching at depth %" PRId8 "\n",
			    coord->name, solution_list.nsols, d);

		arg.target_depth = d;
		solution_moves_reset(arg.solution_moves);
		ndepth = solve_coord_dfs(&arg);

		if (ndepth < 0)
			return ndepth;

		solution_list.nsols += (uint64_t)ndepth;
	}

solve_coord_done:
	return (int64_t)solution_list.nsols;

solve_coord_error_data:
	LOG("[%s solve] Error reading table\n", coord->name);
	return NISSY_ERROR_DATA;

solve_coord_error_buffer:
	LOG("[%s solve] Error appending solution to buffer: size too small\n",
	    coord->name);
	return NISSY_ERROR_BUFFER_SIZE;

solve_coord_error_unsolvable:
	LOG("[%s solve] Error: cube not ready\n", coord->name);
	return NISSY_ERROR_UNSOLVABLE_CUBE;
}
