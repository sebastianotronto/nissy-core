#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "nissy.h"
#include "utils/utils.h"
#include "arch/arch.h"
#include "core/core.h"
#include "solvers/solvers.h"

STATIC long long write_result(oriented_cube_t, char [static NISSY_SIZE_CUBE]);
STATIC long long nissy_dataid(const char *, char [static NISSY_SIZE_DATAID]);
STATIC long long nissy_gendata_unsafe(
    const char *, unsigned long long, unsigned char *);

#define GETCUBE_OPTIONS(S, F) { .option = S, .fix = F }
struct {
	char *option;
	void (*fix)(long long *, long long *,
	    long long *, long long *, long long *);
} getcube_options[] = {
	GETCUBE_OPTIONS("fix", getcube_fix),
	GETCUBE_OPTIONS(NULL, NULL)
};

STATIC long long
write_result(oriented_cube_t cube, char result[static NISSY_SIZE_CUBE])
{
	writecube(cube, NISSY_SIZE_CUBE, result);

	if (!issolvable(cube)) {
		LOG("Warning: resulting cube is not solvable\n");
		return NISSY_WARNING_UNSOLVABLE;
	}

	return NISSY_OK;
}

long long
nissy_inverse(
	const char cube[static NISSY_SIZE_CUBE],
	char result[static NISSY_SIZE_CUBE]
)
{
	oriented_cube_t c, res;
	long long err;

	c = readcube(cube);

	if (iserror(c)) {
		LOG("[inverse] Error: the given cube is invalid\n");
		err = NISSY_ERROR_INVALID_CUBE;
		goto nissy_inverse_error;
	}

	res = (oriented_cube_t) {
		.cube = inverse(c.cube),
		.orientation = c.orientation
	};

	if (!isconsistent(res)) {
		LOG("[inverse] Unknown error: inverted cube is invalid\n");
		err = NISSY_ERROR_UNKNOWN;
		goto nissy_inverse_error;
	}

	return write_result(res, result);

nissy_inverse_error:
	writecube(ZERO_ORIENTED_CUBE, NISSY_SIZE_CUBE, result);
	return err;
}

long long
nissy_applymoves(
	const char cube[static NISSY_SIZE_CUBE],
	const char *moves,
	char result[static NISSY_SIZE_CUBE]
)
{
	oriented_cube_t c, res;
	long long err;

	if (moves == NULL) {
		LOG("[applymoves] Error: 'moves' argument is NULL\n");
		err = NISSY_ERROR_NULL_POINTER;
		goto nissy_applymoves_error;
	}

	c = readcube(cube);

	if (!isconsistent(c)) {
		LOG("[applymoves] Error: given cube is invalid\n");
		err = NISSY_ERROR_INVALID_CUBE;
		goto nissy_applymoves_error;
	}

	res = applymoves(c, moves);

	if (!isconsistent(res)) {
		/* Assume we got a reasonable error message from applymoves */
		err = NISSY_ERROR_INVALID_MOVES;
		goto nissy_applymoves_error;
	}

	return write_result(res, result);

nissy_applymoves_error:
	writecube(ZERO_ORIENTED_CUBE, NISSY_SIZE_CUBE, result);
	return err;
}

long long
nissy_applytrans(
	const char cube[static NISSY_SIZE_CUBE],
	const char transformation[static NISSY_SIZE_TRANSFORMATION],
	char result[static NISSY_SIZE_CUBE]
)
{
	oriented_cube_t c, res;
	long long err;

	c = readcube(cube);

	if (!isconsistent(c)) {
		LOG("[applytrans] Error: given cube is invalid\n");
		err = NISSY_ERROR_INVALID_CUBE;
		goto nissy_applytrans_error;
	}

	res = applytrans(c, transformation);

	if (!isconsistent(res)) {
		/* Assume we got a reasonable error message from applytrans */
		err = NISSY_ERROR_INVALID_TRANS;
		goto nissy_applytrans_error;
	}

	return write_result(res, result);

nissy_applytrans_error:
	writecube(ZERO_ORIENTED_CUBE, NISSY_SIZE_CUBE, result);
	return err;
}

long long
nissy_getcube(
	long long ep,
	long long eo,
	long long cp,
	long long co,
	long long orient,
	const char *options,
	char result[static NISSY_SIZE_CUBE]
)
{
	int i;
	oriented_cube_t oc;

	if (options == NULL) {
		LOG("[getcube] Error: 'options' argument is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	for (i = 0; getcube_options[i].option != NULL; i++)
		if (!strcmp(options, getcube_options[i].option))
			getcube_options[i].fix(&ep, &eo, &cp, &co, &orient);

	oc.cube = getcube(ep, eo, cp, co);
	oc.orientation = orient;

	if (!isconsistent(oc)) {
		LOG("[getcube] Error: could not get cube with ep=%lld, "
		    "eo=%lld, cp=%lld, co=%lld, orient=%lld.\n",
		    ep, eo, cp, co, orient);
		return NISSY_ERROR_OPTIONS;
	}

	return write_result(oc, result);
}

STATIC long long
nissy_dataid(const char *solver, char dataid[static NISSY_SIZE_DATAID])
{
	solver_dispatch_t *dispatch;

	dispatch = match_solver(solver);
	if (dispatch == NULL) {
		LOG("[dataid] Unknown solver %s\n", solver);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	return dispatch->dataid(solver, dataid);
}

long long
nissy_solverinfo(
	const char *solver,
	char dataid[static NISSY_SIZE_DATAID]
)
{
	long long err;
	if ((err = nissy_dataid(solver, dataid)) != NISSY_OK)
		return err;

	/* gendata() handles a NULL *data as a "dryrun" request */
	return nissy_gendata_unsafe(solver, 0, NULL);
}

long long
nissy_gendata(
	const char *solver,
	unsigned long long data_size,
	unsigned char data[data_size]
)
{
	return nissy_gendata_unsafe(solver, data_size, data);
}

STATIC long long
nissy_gendata_unsafe(
	const char *solver,
	unsigned long long data_size,
	unsigned char *data
)
{
	solver_dispatch_t *dispatch;

	if (solver == NULL) {
		LOG("[gendata] Error: 'solver' argument is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	if ((size_t)data % 8 != 0) {
		LOG("[gendata] Error: buffer is not 8-byte aligned\n");
		return NISSY_ERROR_DATA;
	}

	dispatch = match_solver(solver);
	if (dispatch == NULL) {
		LOG("[gendata] Unknown solver %s\n", solver);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	return dispatch->gendata(solver, data_size, data);
}

long long
nissy_checkdata(
	const char *solver,
	unsigned long long data_size,
	const unsigned char data[data_size]
)
{
	solver_dispatch_t *dispatch;

	dispatch = match_solver(solver);
	if (dispatch == NULL) {
		LOG("[checkdata] Unknown solver %s\n", solver);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	return dispatch->checkdata(solver, data_size, data);
}

long long
nissy_solve(
	const char cube[static NISSY_SIZE_CUBE],
	const char *solver, 
	unsigned nissflag,
	unsigned minmoves,
	unsigned maxmoves,
	unsigned maxsols,
	unsigned optimal,
	unsigned threads,
	unsigned long long data_size,
	const unsigned char data[data_size],
	unsigned sols_size,
	char sols[sols_size],
	long long stats[static NISSY_SIZE_SOLVE_STATS],
	int (*poll_status)(void *),
	void *poll_status_data
)
{
	oriented_cube_t oc;
	int t;
	solver_dispatch_t *dispatch;

	if (solver == NULL) {
		LOG("[solve] Error: 'solver' argument is NULL\n");
		return NISSY_ERROR_NULL_POINTER;
	}

	oc = readcube(cube);

	if (!isconsistent(oc)) {
		LOG("[solve] Error: cube is invalid\n");
		return NISSY_ERROR_INVALID_CUBE;
	}

	if (maxmoves > 20) {
		LOG("[solve] 'maxmoves' larger than 20 not supported yet, "
		    "setting it to 20\n");
		maxmoves = 20;
	}

	if (minmoves > maxmoves) {
		LOG("[solve] value provided for 'minmoves' (%u) is larger "
		    "than that provided for 'maxmoves' (%u), setting "
		    "'minmoves' to %u\n", minmoves, maxmoves, maxmoves);
		minmoves = maxmoves;
	}

	if (maxsols == 0) {
		LOG("[solve] 'maxsols' is 0, returning no solution\n");
		return 0;
	}

	if (threads > THREADS)
		LOG("[solve] Selected number of threads (%u) is above the "
		    "maximum value (%d), using %d threads instead\n",
		    threads, THREADS, THREADS);
	t = threads == 0 ? THREADS : MIN(THREADS, threads);

	if ((size_t)data % 8 != 0) {
		LOG("[solve] Error: data buffer is not 8-byte aligned\n");
		return NISSY_ERROR_DATA;
	}

	dispatch = match_solver(solver);
	if (dispatch == NULL) {
		LOG("[solve] Error: unknown solver '%s'\n", solver);
		return NISSY_ERROR_INVALID_SOLVER;
	}
	return dispatch->solve(oc, solver, nissflag, minmoves, maxmoves,
	    maxsols, optimal, t, data_size, data, sols_size, sols, stats,
	    poll_status, poll_status_data);
}

long long
nissy_countmoves(
	const char *moves
)
{
	if (moves == NULL)
		return NISSY_ERROR_NULL_POINTER;

	return countmoves(moves);
}

long long
nissy_setlogger(
	void (*log)(const char *, void *),
	void *user_data
)
{
	nissy_log = log;
	nissy_log_data = user_data;
	return NISSY_OK;
}
