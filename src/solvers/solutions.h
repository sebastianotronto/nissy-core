STATIC void solution_moves_reset(solution_moves_t [static 1]);
STATIC void solution_moves_transform(solution_moves_t [static 1], uint8_t);
STATIC void solution_moves_reorient(solution_moves_t [static 1], uint8_t);
STATIC bool solution_list_init(solution_list_t [static 1], size_t, char *);
STATIC bool solution_moves_equal(
    const solution_moves_t [static 1], const solution_moves_t [static 1]);
STATIC bool solution_moves_is_duplicate(size_t, const solution_moves_t *);
STATIC bool appendchar(solution_list_t [static 1], char);
STATIC bool appendnormal(
    const solution_moves_t [static 1], solution_list_t [static 1]);
STATIC bool appendinverse(
    const solution_moves_t [static 1], solution_list_t [static 1]);
STATIC int64_t appendsolution(const solution_moves_t [static 1],
    const solution_settings_t [static 1], solution_list_t [static 1]);
STATIC bool solutions_done(const solution_list_t [static 1],
    const solution_settings_t [static 1], int8_t depth);

STATIC void
solution_moves_reset(solution_moves_t sol[static 1])
{
	sol->nmoves = 0;
	sol->npremoves = 0;
}

STATIC void
solution_moves_transform(solution_moves_t moves[static 1], uint8_t t)
{
	uint8_t i;

	for (i = 0; i < moves->nmoves; i++)
		moves->moves[i] = transform_move(moves->moves[i], t);

	for (i = 0; i < moves->npremoves; i++)
		moves->premoves[i] = transform_move(moves->premoves[i], t);
}

STATIC void
solution_moves_reorient(solution_moves_t moves[static 1], uint8_t or)
{
	uint8_t i;

	for (i = 0; i < moves->nmoves; i++)
		moves->moves[i] =
		    inverse_reorient_move(moves->moves[i], or);

	for (i = 0; i < moves->npremoves; i++)
		moves->premoves[i] =
		    inverse_reorient_move(moves->premoves[i], or);
}

STATIC bool
solution_list_init(solution_list_t sols[static 1], size_t n, char *buf)
{
	if (n == 0)
		return false;

	sols->nsols = 0;
	sols->shortest_sol = MAXLEN + 1;
	sols->size = n;
	sols->used = 0;
	sols->buf = buf;
	sols->buf[0] = '\0';

	return true;
}

STATIC bool
solution_moves_equal(
	const solution_moves_t a[static 1],
	const solution_moves_t b[static 1]
)
{
	uint8_t i;

	if (a->nmoves != b->nmoves || a->npremoves != b->npremoves)
		return false;

	for (i = 0; i < a->nmoves; i++)
		if (a->moves[i] != b->moves[i])
			return false;

	for (i = 0; i < a->npremoves; i++)
		if (a->premoves[i] != b->premoves[i])
			return false;

	return true;
}

STATIC bool
solution_moves_is_duplicate(size_t n, const solution_moves_t *s)
{
	size_t i;

	for (i = 0; i < n; i++)
		if (solution_moves_equal(&s[i], &s[n]))
			return true;

	return false;
}

STATIC bool
appendchar(solution_list_t solutions[static 1], char c)
{
	if (solutions->size <= solutions->used)
		return false;

	solutions->buf[solutions->used++] = c;

	return true;
}

STATIC bool
appendnormal(
	const solution_moves_t moves[static 1],
	solution_list_t list[static 1]
)
{
	int64_t strl;

	if (moves->nmoves == 0)
		return true;

	if ((strl = writemoves(moves->nmoves, moves->moves,
	    list->size - list->used, list->buf + list->used)) < 0)
		return false;
	list->used += strl;

	return true;
}

STATIC bool
appendinverse(
	const solution_moves_t moves[static 1],
	solution_list_t list[static 1]
)
{
	int64_t strl;

	if (moves->npremoves == 0)
		return true;

	if (!appendchar(list, '('))
		return false;

	if ((strl = writemoves(moves->npremoves, moves->premoves,
	    list->size - list->used, list->buf + list->used)) < 0)
		return false;
	list->used += strl;

	return appendchar(list, ')');
}

STATIC int64_t
appendsolution(
	const solution_moves_t moves[static 1],
	const solution_settings_t settings[static 1],
	solution_list_t list[static 1]
)
{
	int64_t r;
	int i;
	uint8_t t;
	solution_moves_t tsol[NTRANS];

	if (moves->nmoves + moves->npremoves > MAXLEN)
		goto appendsolution_error_solution_length;

	for (
	    t = 0, r = 0;
	    t < NTRANS && list->nsols < settings->maxsolutions;
	    t++
	) {
		if (!(settings->tmask & TM_SINGLE(t)))
			continue;

		tsol[r] = *moves;
		if (settings->unniss) {
			tsol[r].nmoves += moves->npremoves;
			tsol[r].npremoves = 0;
			for (i = moves->npremoves-1; i >= 0; i--)
				tsol[r].moves[tsol[r].nmoves - i - 1] =
				    inverse_move(moves->premoves[i]);

			/*
			This is a bit ugly: we have to sort now and then again
			later, because the allowedmoves check would fail with
			improperly sorted parallel moves, but then transforming
			could swap the pairs the wrong way around.
			*/
			sortparallel_moves(tsol[r].nmoves, tsol[r].moves);

			/* Check if unnissed premoves cancel with normal. */
			if (!allowedmoves(tsol[r].nmoves, tsol[r].moves))
				continue;
		}
		solution_moves_transform(&tsol[r], t);
		solution_moves_reorient(&tsol[r], settings->orientation);
		sortparallel_moves(tsol[r].nmoves, tsol[r].moves);
		sortparallel_moves(tsol[r].npremoves, tsol[r].premoves);

		/* Skip duplicates that may appear after transforming */
		if (solution_moves_is_duplicate(r, tsol))
			continue;

		/* Append first the moves on the side that has more */
		/* E.g. write (U L F) B instead of B (U L F) */
		if (tsol[r].nmoves >= tsol[r].npremoves) {
			if (!appendnormal(&tsol[r], list))
				goto appendsolution_error_buffer;

			if (tsol[r].nmoves > 0 && tsol[r].npremoves > 0)
				if (!appendchar(list, ' '))
					return false;

			if (!appendinverse(&tsol[r], list))
				goto appendsolution_error_buffer;
		} else {
			if (!appendinverse(&tsol[r], list))
				goto appendsolution_error_buffer;

			if (tsol[r].nmoves > 0 && tsol[r].npremoves > 0)
				if (!appendchar(list, ' '))
					return false;

			if (!appendnormal(&tsol[r], list))
				goto appendsolution_error_buffer;
		}

		if (!appendchar(list, '\n'))
			goto appendsolution_error_buffer;

		++list->nsols;
		list->shortest_sol = MIN(
		    list->shortest_sol, tsol[r].nmoves + tsol[r].npremoves);
		r++;

	}

	list->buf[list->used] = '\0';
	return r;

appendsolution_error_buffer:
	list->buf[0] = '\0';
	return NISSY_ERROR_BUFFER_SIZE;

appendsolution_error_solution_length:
	list->buf[0] = '\0';
	return NISSY_ERROR_UNKNOWN;
}

STATIC bool
solutions_done(
	const solution_list_t list[static 1],
	const solution_settings_t settings[static 1],
	int8_t depth
)
{
	return depth > settings->maxmoves ||
	    depth > list->shortest_sol + settings->optimal ||
	    list->nsols >= settings->maxsolutions;
}
