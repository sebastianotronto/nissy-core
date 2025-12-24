#define H48_STARTING_MOVES 4

#if H48_STARTING_MOVES == 3
#define H48_STARTING_CUBES 3240 /* Number of 3-move sequences */
#elif H48_STARTING_MOVES == 4
#define H48_STARTING_CUBES 43254 /* Number of 4-move sequences */
#endif

#define H48_SORT_TASKS_MIN_DEPTH 16
#define H48_LOG_PROGRESS_MIN_DEPTH 15

typedef struct {
	cube_t cube;
	uint8_t moves[H48_STARTING_MOVES];
	int64_t rank;
	uint64_t tmask[H48_STARTING_MOVES];
} solve_h48_task_t;

typedef struct {
	cube_t start_cube;
	cube_t cube;
	cube_t inverse;
	int8_t target_depth;
	solution_moves_t *solution_moves;
	solution_settings_t *solution_settings;
	const uint64_t *tmask;
	solution_list_t *solution_list;
	int8_t lb_normal;
	int8_t lb_inverse;
	bool use_lb_normal; /* TODO remove? */
	bool use_lb_inverse; /* TODO remove? */
	uint8_t h;
	uint8_t base;
	const uint32_t *cocsepdata;
	const unsigned char *h48data;
	const unsigned char *h48data_fallback_eoesep;
	uint64_t movemask_normal;
	uint64_t movemask_inverse;
	uint64_t nodes_visited;
	uint64_t table_fallbacks;
	uint64_t table_lookups;
	int8_t threads;
	int ntasks;
	solve_h48_task_t *tasks;
	int thread_id;
	wrapthread_define_struct_mutex_t(*solutions_mutex);
	wrapthread_atomic int *status;
	wrapthread_atomic bool thread_done;
} dfsarg_solve_h48_t;

typedef struct {
	cube_t cube;
	int8_t nmoves;
	uint8_t moves[H48_STARTING_MOVES];
	int8_t minmoves;
	int8_t maxmoves;
	int8_t *shortest_sol;
	uint64_t tmask[H48_STARTING_MOVES];
} dfsarg_solve_h48_maketasks_t;

typedef struct {
	uint8_t stop;
	uint8_t pn;
	uint8_t pi;
	uint8_t lookups;
	uint8_t fallbacks;
	uint8_t nohalf_normal;
	uint8_t nohalf_inverse;
} solve_h48_prune_return_t;

typedef struct {
	cube_t cube;
	cube_t inverse;
	const uint32_t *cocsepdata;
	const unsigned char *h48data;
	const unsigned char *eoesepdata;
	uint8_t target;
	uint8_t h48base;
	uint8_t h48h;
	uint8_t lb_inverse;
} solve_h48_prune_arg_t;

STATIC long long solve_h48_dispatch(oriented_cube_t, const char *, unsigned,
    unsigned, unsigned, unsigned, unsigned, unsigned, unsigned long long,
    const unsigned char *, unsigned, char *,
    long long [static NISSY_SIZE_SOLVE_STATS], int (*)(void *), void *);
STATIC_INLINE bool solve_h48_stop(dfsarg_solve_h48_t [static 1]);
STATIC_INLINE solve_h48_prune_return_t solve_h48_prune(solve_h48_prune_arg_t);
STATIC int64_t solve_h48_maketasks(
    dfsarg_solve_h48_t [static 1], dfsarg_solve_h48_maketasks_t [static 1],
    solve_h48_task_t [static H48_STARTING_CUBES], int [static 1]);
STATIC void *solve_h48_runthread(void *);
STATIC int64_t solve_h48_dfs(dfsarg_solve_h48_t [static 1]);
STATIC void solve_h48_log_solutions(solution_list_t [static 1], size_t);
STATIC int solve_h48_compare_tasks(const void *, const void *);
STATIC int64_t solve_h48(oriented_cube_t, uint8_t, uint8_t, uint64_t, uint8_t,
    uint8_t, uint64_t, const unsigned char *, size_t, char *,
    long long [static NISSY_SIZE_SOLVE_STATS], int (*)(void *), void *);

STATIC long long solve_h48_dispatch(
	oriented_cube_t oc,
        const char *solver,
        unsigned nissflag,
        unsigned minmoves,
        unsigned maxmoves,
        unsigned maxsols,
        unsigned optimal,
        unsigned threads,
        unsigned long long data_size,
        const unsigned char *data,
        unsigned sols_size,
        char *sols,
        long long stats[static NISSY_SIZE_SOLVE_STATS],
        int (*poll_status)(void *),
        void *poll_status_data
)
{
	uint8_t h;
	long long err;

	err = parse_h48h(solver, &h);
	if (err != NISSY_OK)
		return err;

	return solve_h48(oc, minmoves, maxmoves, maxsols, optimal, threads,
	    data_size, data, sols_size, sols, stats,
	    poll_status, poll_status_data);
}

STATIC_INLINE bool
solve_h48_stop(dfsarg_solve_h48_t arg[static 1])
{
	uint32_t data, data_inv;
	int64_t coord;
	int8_t target, nh, n;
	uint8_t pval, pval_min, pval_eoesep;

	arg->movemask_normal = arg->movemask_inverse = MM18_ALLMOVES;

	n = arg->solution_moves->nmoves + arg->solution_moves->npremoves;
	target = arg->target_depth - n;

	/* We'll never get a bound higher than base + 3 */
	if (arg->base + 3 <= target)
		return false;

	/* Preliminary probing using last computed bound, if possible */
	if ((arg->use_lb_normal && arg->lb_normal > target) ||
	    (arg->use_lb_inverse && arg->lb_inverse > target))
		return true;

	/* Get cdata and do preliminary corner probing */
	if (get_h48_cdata(arg->cube, arg->cocsepdata, &data) > target ||
	    get_h48_cdata(arg->inverse, arg->cocsepdata, &data_inv) > target)
		return true;

	/* Inverse probing */

	if (!arg->use_lb_inverse) {
		arg->table_lookups++;
		arg->use_lb_inverse = true;
		coord = coord_h48_edges(
		    arg->inverse, COCLASS(data_inv), TTREP(data_inv), arg->h);
		pval = get_h48_pval_and_min(arg->h48data, coord, &pval_min);

		if (pval == 0) {
			arg->table_fallbacks++;

			pval_eoesep = get_eoesep_pval_cube(
			    arg->h48data_fallback_eoesep, arg->inverse);
			pval = MAX(pval_min, pval_eoesep);
		} else {
			pval += arg->base;
		}

		arg->lb_inverse = pval;
	}

	if (arg->lb_inverse > target)
		return true;
	nh = arg->lb_inverse == target;
	arg->movemask_normal = nh * MM18_NOHALFTURNS + (1-nh) * MM18_ALLMOVES;

	/* Normal probing */

	if (!arg->use_lb_normal) {
		arg->table_lookups++;
		arg->use_lb_normal = true;
		coord = coord_h48_edges(
		    arg->cube, COCLASS(data), TTREP(data), arg->h);
		pval = get_h48_pval_and_min(arg->h48data, coord, &pval_min);

		if (pval == 0) {
			arg->table_fallbacks++;

			pval_eoesep = get_eoesep_pval_cube(
			    arg->h48data_fallback_eoesep, arg->cube);
			pval = MAX(pval_min, pval_eoesep);
		} else {
			pval += arg->base;
		}

		arg->lb_normal = pval;
	}

	if (arg->lb_normal > target)
		return true;
	nh = arg->lb_normal == target;
	arg->movemask_inverse = nh * MM18_NOHALFTURNS + (1-nh) * MM18_ALLMOVES;

	return false;
}

STATIC_INLINE solve_h48_prune_return_t
solve_h48_prune(solve_h48_prune_arg_t arg)
{
	solve_h48_prune_return_t ret = {0};
	uint64_t c;
	uint32_t dn, di;
	uint8_t pmin, pe;

	ret.pi = arg.lb_inverse;

	/* We'll never get a bound higher than base + 3 */
	if (arg.h48base + 3 <= arg.target)
		goto solve_h48_prune_return_false;

	/* Preliminary probing using last computed bound, if possible */
	if (arg.lb_inverse > arg.target)
		goto solve_h48_prune_return_true;

	/* Get cdata and do preliminary corner probing */
	if (get_h48_cdata(arg.inverse, arg.cocsepdata, &di) > arg.target ||
	    get_h48_cdata(arg.cube, arg.cocsepdata, &dn) > arg.target)
		goto solve_h48_prune_return_true;

	if (arg.lb_inverse == 0) {
		ret.lookups++;
		c = coord_h48_edges(
		    arg.inverse, COCLASS(di), TTREP(di), arg.h48h);
		ret.pi = get_h48_pval_and_min(arg.h48data, c, &pmin);

		if (ret.pi == 0) {
			ret.fallbacks++;
			pe = get_eoesep_pval_cube(arg.eoesepdata, arg.inverse);
			ret.pi = MAX(pmin, pe);
		} else {
			ret.pi += arg.h48base;
		}
	}

	if (ret.pi > arg.target)
		goto solve_h48_prune_return_true;

	ret.nohalf_normal = ret.pi == arg.target;

	ret.lookups++;
	c = coord_h48_edges(arg.cube, COCLASS(dn), TTREP(dn), arg.h48h);
	ret.pn = get_h48_pval_and_min(arg.h48data, c, &pmin);

	if (ret.pn == 0) {
		ret.fallbacks++;
		pe = get_eoesep_pval_cube(arg.eoesepdata, arg.cube);
		ret.pn = MAX(pmin, pe);
	} else {
		ret.pn += arg.h48base;
	}

	if (ret.pn > arg.target)
		goto solve_h48_prune_return_true;

	ret.nohalf_inverse = ret.pn == arg.target;

solve_h48_prune_return_false:
	ret.stop = false;
	return ret;

solve_h48_prune_return_true:
	ret.stop = true;
	return ret;
}

#if 1

STATIC int64_t
solve_h48_dfs(dfsarg_solve_h48_t arg[static 1])
{
	int64_t ret, n;
	uint8_t m, nm, nn, ni, lbn, lbi;
	uint64_t mm_normal, mm_inverse;
	cube_t backup_cube, backup_inverse;
	solve_h48_prune_arg_t prune_arg;
	solve_h48_prune_return_t prune;

	nn = arg->solution_moves->nmoves;
	ni = arg->solution_moves->npremoves;
	nm = nn + ni;
	if (equal(arg->cube, SOLVED_CUBE)) {
		if (arg->target_depth != nm)
			return 0;
		wrapthread_mutex_lock(arg->solutions_mutex);
		ret = appendsolution(arg->solution_moves, H48_STARTING_MOVES,
		    arg->tmask, arg->solution_settings, arg->solution_list);
		wrapthread_mutex_unlock(arg->solutions_mutex);
		return ret;
	}

	if (nm + 1 > arg->target_depth ||
	    arg->solution_list->nsols >= arg->solution_settings->maxsolutions)
		return 0;

	backup_cube = arg->cube;
	backup_inverse = arg->inverse;
	lbn = arg->lb_normal;
	lbi = arg->lb_inverse;
	mm_normal = arg->movemask_normal;
	mm_inverse = arg->movemask_inverse;

	ret = 0;

	prune_arg = (solve_h48_prune_arg_t){
		.cocsepdata = arg->cocsepdata,
		.h48data = arg->h48data,
		.eoesepdata = arg->h48data_fallback_eoesep, /* TODO rmove? */
		.target = arg->target_depth - (nm + 1),
		.h48base = arg->base,
		.h48h = arg->h,
		.lb_inverse = 0,
	};
	if (popcount_u32(mm_normal) <= popcount_u32(mm_inverse)) {
		arg->solution_moves->nmoves++;
		for (m = 0; m < 18; m++) {
			if (!(mm_normal & MM_SINGLE(m)))
				continue;

			prune_arg.cube = move(backup_cube, m);
			prune_arg.inverse = premove(backup_inverse, m);
			prune_arg.lb_inverse = m % 3 == 1 ? lbi : 0;

			prune = solve_h48_prune(prune_arg);

			arg->nodes_visited++;
			arg->table_lookups += prune.lookups;
			arg->table_fallbacks += prune.fallbacks;

			if (prune.stop)
				continue;

			arg->solution_moves->moves[nn] = m;
			arg->cube = prune_arg.cube;
			arg->inverse = prune_arg.inverse;
			arg->lb_inverse = prune.pi;
			arg->lb_normal = prune.pn;
			arg->movemask_normal = allowedmask[movebase(m)];
			if (prune.nohalf_normal)
				arg->movemask_normal &= MM18_NOHALFTURNS;
			arg->movemask_inverse = mm_inverse;
			if (prune.nohalf_inverse)
				arg->movemask_inverse &= MM18_NOHALFTURNS;

			n = solve_h48_dfs(arg);

			if (n < 0)
				return n;
			ret += n;
		}
		arg->solution_moves->nmoves--;
	} else {
		arg->solution_moves->npremoves++;
		for (m = 0; m < 18; m++) {
			if(!(mm_inverse & MM_SINGLE(m)))
				continue;

			prune_arg.cube = move(backup_inverse, m);
			prune_arg.inverse = premove(backup_cube, m);
			prune_arg.lb_inverse = m % 3 == 1 ? lbn : 0;
			prune = solve_h48_prune(prune_arg);

			arg->nodes_visited++;
			arg->table_lookups += prune.lookups;
			arg->table_fallbacks += prune.fallbacks;

			if (prune.stop)
				continue;

			arg->solution_moves->premoves[ni] = m;
			arg->inverse = prune_arg.cube;
			arg->cube = prune_arg.inverse;
			arg->lb_normal = prune.pi;
			arg->lb_inverse = prune.pn;
			arg->movemask_normal = mm_normal;
			if (prune.nohalf_inverse)
				arg->movemask_normal &= MM18_NOHALFTURNS;
			arg->movemask_inverse = allowedmask[movebase(m)];
			if (prune.nohalf_normal)
				arg->movemask_inverse &= MM18_NOHALFTURNS;

			n = solve_h48_dfs(arg);

			if (n < 0)
				return n;
			ret += n;
		}
		arg->solution_moves->npremoves--;
	}

	arg->cube = backup_cube;
	arg->inverse = backup_inverse;

	return ret;
}

#else

STATIC int64_t
solve_h48_dfs(dfsarg_solve_h48_t arg[static 1])
{
	int64_t ret, n;
	uint8_t m, nm, lbn, lbi;
	uint64_t mm_normal, mm_inverse;
	bool ulbi, ulbn;
	cube_t backup_cube, backup_inverse;

	arg->nodes_visited++;

	nm = arg->solution_moves->nmoves + arg->solution_moves->npremoves;
	if (equal(arg->cube, SOLVED_CUBE)) {
		if (arg->target_depth != nm)
			return 0;
		wrapthread_mutex_lock(arg->solutions_mutex);
		ret = appendsolution(arg->solution_moves, H48_STARTING_MOVES,
		    arg->tmask, arg->solution_settings, arg->solution_list);
		wrapthread_mutex_unlock(arg->solutions_mutex);
		return ret;
	}

	if (solve_h48_stop(arg))
		return 0;

	if (nm + 1 > arg->target_depth ||
	    arg->solution_list->nsols >= arg->solution_settings->maxsolutions)
		return 0;

	backup_cube = arg->cube;
	backup_inverse = arg->inverse;
	lbn = arg->lb_normal;
	lbi = arg->lb_inverse;
	ulbn = arg->use_lb_normal;
	ulbi = arg->use_lb_inverse;

	ret = 0;
	mm_normal = arg->movemask_normal;
	if (arg->solution_moves->nmoves > 0) {
		m = arg->solution_moves->moves[arg->solution_moves->nmoves-1];
		mm_normal &= allowedmask[movebase(m)];
	}
	mm_inverse = arg->movemask_inverse;
	if (arg->solution_moves->npremoves > 0) {
		m = arg->solution_moves->premoves[arg->solution_moves->npremoves-1];
		mm_inverse &= allowedmask[movebase(m)];
	}
	if (popcount_u32(mm_normal) <= popcount_u32(mm_inverse)) {
		arg->solution_moves->nmoves++;
		for (m = 0; m < 18; m++) {
			if (!(mm_normal & MM_SINGLE(m)))
				continue;
			arg->solution_moves->moves[
			    arg->solution_moves->nmoves-1] = m;
			arg->cube = move(backup_cube, m);
			arg->inverse = premove(backup_inverse, m);
			arg->lb_inverse = lbi;
			arg->use_lb_normal = false;
			arg->use_lb_inverse = ulbi && m % 3 == 1;
			n = solve_h48_dfs(arg);
			if (n < 0)
				return n;
			ret += n;
		}
		arg->solution_moves->nmoves--;
	} else {
		arg->solution_moves->npremoves++;
		for (m = 0; m < 18; m++) {
			if(!(mm_inverse & MM_SINGLE(m)))
				continue;
			arg->solution_moves->premoves[
			    arg->solution_moves->npremoves-1] = m;
			arg->inverse = move(backup_inverse, m);
			arg->cube = premove(backup_cube, m);
			arg->lb_normal = lbn;
			arg->use_lb_inverse = false;
			arg->use_lb_normal = ulbn && m % 3 == 1;
			n = solve_h48_dfs(arg);
			if (n < 0)
				return n;
			ret += n;
		}
		arg->solution_moves->npremoves--;
	}

	arg->cube = backup_cube;
	arg->inverse = backup_inverse;

	return ret;
}

#endif

STATIC void *
solve_h48_runthread(void *arg)
{
	int i, j;
	uint8_t lastmove;
	int64_t nprev;
	dfsarg_solve_h48_t *dfsarg;

	dfsarg = (dfsarg_solve_h48_t *)arg;

	nprev = 0;
	for (i = dfsarg->thread_id; i < dfsarg->ntasks; i += dfsarg->threads) {
		if (*dfsarg->status == NISSY_STATUS_STOP)
			goto solve_h48_runthread_end;
		while (*dfsarg->status == NISSY_STATUS_PAUSE)
			msleep(BASE_SLEEP_TIME);

		solution_moves_reset(dfsarg->solution_moves);
		memcpy(dfsarg->solution_moves->moves,
		    dfsarg->tasks[i].moves, H48_STARTING_MOVES);
		dfsarg->solution_moves->nmoves = H48_STARTING_MOVES;

		dfsarg->cube = dfsarg->start_cube;
		for (j = 0; j < H48_STARTING_MOVES; j++)
			dfsarg->cube =
			   move(dfsarg->cube, dfsarg->tasks[i].moves[j]);
		dfsarg->inverse = inverse(dfsarg->cube);

		dfsarg->lb_normal = 0;
		dfsarg->lb_inverse = 0;
		dfsarg->use_lb_normal = false;
		dfsarg->use_lb_inverse = false;
		dfsarg->movemask_normal = allowedmask[
		    movebase(dfsarg->tasks[i].moves[H48_STARTING_MOVES-1])];
		dfsarg->movemask_inverse = MM18_ALLMOVES;
		dfsarg->tmask = dfsarg->tasks[i].tmask;

		solve_h48_dfs(dfsarg);

		/*
		We compute the "rank" of each taks, which is used in the next
		step of the IDFS to sort them. This heuristically leads us
		faster to a solution.  The rank is computed by taking the
		number of nodes visited, adjusted by a factor of about
		sqrt(2)/sqrt(3) because there are more sequences starting with
		U, R and F than with D, L and B, as we don't allow e.g. both
		U D and D U, but only U D.
		This trick was suggested by Chen Shuang, the implementation is
		inspired by Andrew Skalski's vcube.
		*/
		lastmove = dfsarg->tasks[i].moves[H48_STARTING_MOVES-1];
		dfsarg->tasks[i].rank = (dfsarg->nodes_visited - nprev) *
		    (movebase(lastmove) % 2 == 0 ? 47525 : 58206);
		nprev = dfsarg->nodes_visited;
	}

solve_h48_runthread_end:
	dfsarg->thread_done = true;
	return NULL;
}

STATIC int64_t
solve_h48_maketasks(
	dfsarg_solve_h48_t solve_arg[static 1],
	dfsarg_solve_h48_maketasks_t mtarg[static 1],
	solve_h48_task_t tasks[static H48_STARTING_CUBES],
	int ntasks[static 1]
)
{
	int r;
	int64_t appret;
	uint8_t m, t;
	uint64_t mm;
	cube_t backup_cube;
	solution_moves_t moves;

	if (equal(mtarg->cube, SOLVED_CUBE)) {
		if (mtarg->nmoves > mtarg->maxmoves ||
		    mtarg->nmoves < mtarg->minmoves ||
		    solutions_done(solve_arg->solution_list,
		        solve_arg->solution_settings, mtarg->nmoves))
			return NISSY_OK;

		solution_moves_reset(&moves);
		moves.nmoves = mtarg->nmoves;
		memcpy(moves.moves, mtarg->moves, mtarg->nmoves);

		appret = appendsolution(&moves, mtarg->nmoves, mtarg->tmask,
		    solve_arg->solution_settings, solve_arg->solution_list);
		return appret < 0 ? appret : NISSY_OK;
	}

	if (mtarg->nmoves == H48_STARTING_MOVES) {
		tasks[*ntasks].cube = mtarg->cube;
		memcpy(tasks[*ntasks].moves, mtarg->moves,
		    H48_STARTING_MOVES * sizeof(uint8_t));
		memcpy(tasks[*ntasks].tmask, mtarg->tmask,
		    H48_STARTING_MOVES * sizeof(uint64_t));
		(*ntasks)++;
		return NISSY_OK;
	}

	if (mtarg->nmoves == 0) {
		mm = MM18_ALLMOVES;
	} else {
		m = mtarg->moves[mtarg->nmoves-1];
		mm = allowedmask[movebase(m)];
	}

	mtarg->tmask[mtarg->nmoves] = symmetry_mask(mtarg->cube);

	mtarg->nmoves++;
	backup_cube = mtarg->cube;
	for (m = 0; m < 18; m++) {
		if (!(mm & MM_SINGLE(m)))
			continue;

		mtarg->moves[mtarg->nmoves-1] = m;
		mtarg->cube = move(backup_cube, m);
		r = solve_h48_maketasks(solve_arg, mtarg, tasks, ntasks);
		if (r < 0)
			return r;

		/* Avoid symmetry-equivalent moves from the starting cube */
		for (t = 0; t < NTRANS; t++)
			if (mtarg->tmask[mtarg->nmoves-1] & TM_SINGLE(t))
				mm &= ~MM_SINGLE(transform_move(m, t));
	}
	mtarg->nmoves--;
	mtarg->cube = backup_cube;

	return NISSY_OK;
}

STATIC void
solve_h48_log_solutions(solution_list_t s[static 1], size_t e)
{
	size_t i;
	char b;
	while (e != s->used) {
		LOG("[h48 solve] Found solution: ");
		for (i = e; s->buf[i] != '\n' && s->buf[i] != '\0'; i++) ;
		b = s->buf[i];
		s->buf[i] = '\0';
		LOG("%s\n", s->buf + e);
		s->buf[i] = b;
		e = i + 1;
	}
}

STATIC int
solve_h48_compare_tasks(const void *x, const void *y)
{
	int64_t nodes_x = ((solve_h48_task_t *)x)->rank;
	int64_t nodes_y = ((solve_h48_task_t *)y)->rank;

	/* Same as returning nodes_y - nodes_x, but avoids overflow */
	return (nodes_x < nodes_y) - (nodes_x > nodes_y);
}

STATIC int64_t
solve_h48(
	oriented_cube_t oc,
	uint8_t minmoves,
	uint8_t maxmoves,
	uint64_t maxsolutions,
	uint8_t optimal,
	uint8_t threads,
	uint64_t data_size,
	const unsigned char *data,
	size_t solutions_size,
	char *solutions,
	long long stats[static NISSY_SIZE_SOLVE_STATS],
	int (*poll_status)(void *),
	void *poll_status_data
)
{
	int i, ntasks;
	bool td;
	wrapthread_atomic int status, prev_status;
	size_t lastused;
	int8_t d;
	dfsarg_solve_h48_t arg[THREADS];
	solve_h48_task_t tasks[H48_STARTING_CUBES];
	dfsarg_solve_h48_maketasks_t mtarg;
	long double fallback_rate, lookups_per_node;
	uint64_t offset;
	uint64_t nodes_visited, table_lookups, table_fallbacks;
	tableinfo_t info, fbinfo2;
	const uint32_t *cocsepdata;
	const unsigned char *h48data;
	const unsigned char *eoesep;
	solution_moves_t solution_moves[THREADS];
	solution_settings_t settings;
	solution_list_t sollist;
	wrapthread_define_var_thread_t(thread[THREADS]);
	wrapthread_define_var_mutex_t(solutions_mutex);

	if (!solution_list_init(&sollist, solutions_size, solutions))
		goto solve_h48_error_solutions_buffer;

	if (readtableinfo_n(data_size, data, 2, &info) != NISSY_OK)
		goto solve_h48_error_data;

	cocsepdata = (uint32_t *)(data + INFOSIZE);
	h48data = data + COCSEP_FULLSIZE + INFOSIZE;

	/* Read additional eoesep table */
	offset = info.next;
	if (readtableinfo_n(data_size, data, 3, &fbinfo2)
	    != NISSY_OK)
		goto solve_h48_error_data;

	/* Some heuristic check to see that it is eoesep */
	if (fbinfo2.bits != 4 || fbinfo2.type != TABLETYPE_SPECIAL)
		goto solve_h48_error_data;
	eoesep = h48data + offset;

	settings = (solution_settings_t) {
		.unniss = true,
		.maxmoves = maxmoves,
		.maxsolutions = maxsolutions,
		.optimal = optimal,
		.orientation = oc.orientation,
	};

	for (i = 0; i < threads; i++) {
		arg[i] = (dfsarg_solve_h48_t) {
			.start_cube = oc.cube,
			.cube = oc.cube,
			.h = info.h48h,
			.base = info.base,
			.cocsepdata = cocsepdata,
			.h48data = h48data,
			.h48data_fallback_eoesep = eoesep,
			.solution_moves = &solution_moves[i],
			.solution_settings = &settings,
			.solution_list = &sollist,
			.nodes_visited = 0,
			.table_fallbacks = 0,
			.table_lookups = 0,
			.threads = threads,
			.thread_id = i,
			.solutions_mutex = &solutions_mutex,
			.status = &status,
		};

	}

	wrapthread_mutex_init(&solutions_mutex, NULL);

	mtarg = (dfsarg_solve_h48_maketasks_t) {
		.cube = oc.cube,
		.nmoves = 0,
		.minmoves = minmoves,
		.maxmoves = maxmoves,
	};
	ntasks = 0;
	solve_h48_maketasks(&arg[0], &mtarg, tasks, &ntasks);
	if (ntasks < 0)
		goto solve_h48_error_solutions_buffer;
	if (solutions_done(&sollist, &settings,
	    MAX(minmoves, H48_STARTING_MOVES)))
		goto solve_h48_done;

	for (i = 0; i < threads; i++) {
		arg[i].ntasks = ntasks;
		arg[i].tasks = tasks;
	}

	LOG("[H48 solve] Prepared %d tasks\n", ntasks);

	solve_h48_log_solutions(&sollist, 0);
	lastused = sollist.used;
	status = poll_status == NULL ? NISSY_STATUS_RUN :
	    poll_status(poll_status_data);
	if (!NISSY_CANSLEEP) {
		LOG("[solve h48] Pause / Stop / Resume functionality won't "
		    "be available on this system (can't sleep()).\n");
	}
	for (
	    d = MAX(minmoves, H48_STARTING_MOVES + 1);
	    !(solutions_done(&sollist, &settings, d)) &&
	        status != NISSY_STATUS_STOP;
	    d++
	) {
		if (d >= H48_LOG_PROGRESS_MIN_DEPTH) {
			LOG("[H48 solve] Found %" PRIu64 " solutions, "
			    "searching at depth %" PRId8 "\n",
			    sollist.nsols, d);
		}

		if (d >= H48_SORT_TASKS_MIN_DEPTH)
			qsort(tasks, ntasks, sizeof(solve_h48_task_t),
			    solve_h48_compare_tasks);

		for (i = 0; i < threads; i++) {
			arg[i].target_depth = d;
			arg[i].thread_done = false;
			wrapthread_create(
			    &thread[i], NULL, solve_h48_runthread, &arg[i]);
		}

		/* Log solutions and handle pause / stop / resume */
		if (poll_status != NULL && d >= 15 && NISSY_CANSLEEP) {
			td = false;
			while (!td && status != NISSY_STATUS_STOP) {
				msleep(BASE_SLEEP_TIME);

				wrapthread_mutex_lock(&solutions_mutex);
				solve_h48_log_solutions(&sollist, lastused);
				lastused = sollist.used;
				wrapthread_mutex_unlock(&solutions_mutex);

				prev_status = status;
				status = poll_status(poll_status_data);
				if (status != prev_status) {
					if (status == NISSY_STATUS_PAUSE)
						LOG("[H48 solve] Paused\n");
					if (status == NISSY_STATUS_RUN)
						LOG("[H48 solve] Resumed\n");
				}

				for (td = true, i = 0; i < threads; i++)
					td = td && arg[i].thread_done;
			}
		}

		for (i = 0; i < threads; i++)
			wrapthread_join(thread[i], NULL);

		solve_h48_log_solutions(&sollist, lastused);
		lastused = sollist.used;
	}

	if (status == NISSY_STATUS_STOP) {
		LOG("[H48 solve] Received stop request, ending solution "
		    "search early.\n");
	}

solve_h48_done:
	nodes_visited = table_lookups = table_fallbacks = 0;
	for (i = 0; i < threads; i++) {
		nodes_visited += arg[i].nodes_visited;
		table_fallbacks += arg[i].table_fallbacks;
		table_lookups += arg[i].table_lookups;
	}

	stats[0] = nodes_visited;
	stats[1] = table_lookups;
	stats[2] = table_fallbacks;
	lookups_per_node = table_lookups / (long double)nodes_visited;
	fallback_rate = nodes_visited == 0 ? 0.0 :
	    (table_fallbacks * 100) / (long double)table_lookups;
	LOG("[H48 solve] Nodes visited: %" PRId64 "\n", nodes_visited);
	LOG("[H48 solve] Lookups: %" PRId64 " (%.3Lf per node)\n",
	    table_lookups, lookups_per_node);
	LOG("[H48 solve] Table fallbacks: %" PRId64 " (%.3Lf%%)\n",
	    table_fallbacks, fallback_rate);

	return sollist.nsols;

solve_h48_error_data:
	LOG("[H48 solve] Error reading data table\n");
	return NISSY_ERROR_DATA;

solve_h48_error_solutions_buffer:
	return NISSY_ERROR_BUFFER_SIZE;
}
