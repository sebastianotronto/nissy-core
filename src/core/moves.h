#define MOVE(M, c) compose(c, MOVE_CUBE_ ## M)
#define PREMOVE(M, c) compose(MOVE_CUBE_ ## M, c)

STATIC uint8_t readmove(char);
STATIC int64_t readmoves(const char *,
    size_t, size_t, uint64_t *, uint64_t *, uint8_t *, uint8_t *);
STATIC int64_t countmoves(const char *);
STATIC uint8_t readmodifier(char);
STATIC int64_t writemoves(size_t, const uint8_t *, size_t, char *);

STATIC_INLINE bool allowednextmove(uint8_t, uint8_t);
STATIC bool allowedmoves(size_t, const uint8_t *);

STATIC_INLINE uint8_t movebase(uint8_t);
STATIC_INLINE uint8_t moveaxis(uint8_t);
STATIC_INLINE bool isbase(uint8_t);
STATIC_INLINE bool parallel(uint8_t, uint8_t);
STATIC_INLINE uint8_t moveopposite(uint8_t);
STATIC_INLINE uint8_t reorient_move(uint8_t, uint8_t);
STATIC_INLINE uint8_t inverse_reorient_move(uint8_t, uint8_t);
STATIC_INLINE uint8_t movefollow(uint8_t);
STATIC uint8_t transform_move(uint8_t, uint8_t);

STATIC cube_t move(cube_t, uint8_t);
STATIC cube_t premove(cube_t, uint8_t);
STATIC uint8_t inverse_move(uint8_t);
STATIC void sortparallel_moves(size_t, uint8_t*);
STATIC bool are_lastmoves_singlecw(size_t, const uint8_t*);

#define FOREACH_READMOVE(ARG_BUF, ARG_MOVE, ARG_C, ARG_MAX, \
	RET_ERROR, ARG_ACTION) \
	const char *VAR_B; \
	uint8_t VAR_MOVE_NOMOD, VAR_MOD; \
	bool VAR_IN_PARENTHESES = false; \
	for (VAR_B = ARG_BUF, ARG_C = 0; *VAR_B != '\0'; VAR_B++, ARG_C++) { \
		while (*VAR_B == ' ' || *VAR_B == '\t' || *VAR_B == '\n') \
			VAR_B++; \
		if (*VAR_B == '\0' || ARG_C == ARG_MAX) \
			break; \
		if (*VAR_B == '(') { \
			if (VAR_IN_PARENTHESES) { \
				LOG("Nested parentheses in move sequence\n"); \
				return RET_ERROR; \
			} \
			VAR_IN_PARENTHESES = true; \
			continue; \
		} \
		if (*VAR_B == ')') { \
			if (!VAR_IN_PARENTHESES) { \
				LOG("Mismatched ')' in move sequence\n"); \
				return RET_ERROR; \
			} \
			VAR_IN_PARENTHESES = false; \
			continue; \
		} \
		if ((VAR_MOVE_NOMOD = readmove(*VAR_B)) == UINT8_ERROR) { \
			LOG("Unknown move: %c\n", *VAR_B); \
			return RET_ERROR; \
		} \
		if (*(VAR_B+1) == 'w') { \
			VAR_MOVE_NOMOD += 18; \
			VAR_B++; \
		} \
		if ((VAR_MOD = readmodifier(*(VAR_B+1))) != 0) \
			VAR_B++; \
		ARG_MOVE = VAR_MOVE_NOMOD + VAR_MOD; \
		ARG_ACTION \
	} \
	if (VAR_IN_PARENTHESES) { \
		LOG("Mismatched '(' in move sequence\n"); \
		return RET_ERROR; \
	}

STATIC uint8_t
readmove(char c)
{
	switch (c) {
	case 'U':
		return MOVE_U;
	case 'D':
		return MOVE_D;
	case 'R':
		return MOVE_R;
	case 'L':
		return MOVE_L;
	case 'F':
		return MOVE_F;
	case 'B':
		return MOVE_B;
	case 'M':
		return MOVE_M;
	case 'S':
		return MOVE_S;
	case 'E':
		return MOVE_E;
	case 'x':
		return MOVE_x;
	case 'y':
		return MOVE_y;
	case 'z':
		return MOVE_z;
	default:
		return UINT8_ERROR;
	}
}

STATIC uint8_t
readmodifier(char c)
{
	switch (c) {
	case '1': /* Fallthrough */
	case '2': /* Fallthrough */
	case '3':
		return c - '0' - 1;
	case '\'':
		return 2;
	default:
		return 0;
	}
}

STATIC int64_t
readmoves(
	const char *buf,
	size_t nsize,
	size_t invsize,
	uint64_t *n,
	uint64_t *i,
	uint8_t *normal,
	uint8_t *inverse
)
{
	uint8_t m;
	uint64_t c;

	*n = *i = 0;
	FOREACH_READMOVE(buf, m, c, nsize+invsize, NISSY_ERROR_INVALID_MOVES,
		if (!VAR_IN_PARENTHESES) {
			if (*n >= nsize-1) {
				LOG("Error in readmoves: normal buffer\n");
				return NISSY_ERROR_BUFFER_SIZE;
			}
			normal[(*n)++] = m;
		} else {
			if (*i >= invsize-1) {
				LOG("Error in readmoves: inverse buffer\n");
				return NISSY_ERROR_BUFFER_SIZE;
			}
			inverse[(*i)++] = m;
		}
	)

	return (int64_t)c;
}

STATIC int64_t
countmoves(const char *buf)
{
	uint8_t m;
	uint64_t c;
	int64_t count;

	count = 0;
	FOREACH_READMOVE(buf, m, c, INT_MAX, NISSY_ERROR_INVALID_MOVES,
		count += m <= MOVE_Bw3 ? 1 : (m <= MOVE_E3 ? 2 : 0);
	)

	return count;
}

STATIC int64_t
writemoves(
	size_t nmoves,
	const uint8_t *m,
	size_t buf_size,
	char *buf
)
{
	size_t i, len, w;
	const char *s;

	if (buf_size == 0) {
		LOG("Error: cannot write moves to buffer of size 0.\n");
		return NISSY_ERROR_BUFFER_SIZE;
	}

	for (i = 0, w = 0; i < nmoves; i++, w++) {
		s = movestr[m[i]];
		len = strlen(s);
		if (len + w >= buf_size) {
			LOG("Error: the given buffer is too small for "
			     "writing the given moves.\n");
			goto writemoves_error;
		}
		memcpy(buf+w, s, len);
		w += len;
		buf[w] = ' ';
	}

	if (w > 0) w--; /* Remove last space */
	buf[w] = '\0';

	return (int64_t)w;

writemoves_error:
	*buf = '\0';
	return NISSY_ERROR_BUFFER_SIZE;
}

STATIC_INLINE bool
allowednextmove(uint8_t m1, uint8_t m2)
{
	return allowedmask[movebase(m1)] & MM_SINGLE(m2);
}

STATIC bool
allowedmoves(size_t n, const uint8_t *m)
{
	uint8_t j;

	for (j = 1; j < n; j++)
		if (!allowednextmove(m[j-1], m[j]))
			return false;

	return true;
}

STATIC_INLINE uint8_t
movebase(uint8_t move)
{
	return move / 3;
}

STATIC_INLINE uint8_t
moveaxis(uint8_t move)
{
	return move / 6;
}

STATIC_INLINE bool
isbase(uint8_t move)
{
	return move == 3 * movebase(move);
}

STATIC_INLINE bool
parallel(uint8_t m1, uint8_t m2)
{
	return moveaxis(movefollow(m1)) == moveaxis(movefollow(m2));
}

STATIC_INLINE uint8_t
moveopposite(uint8_t move)
{
	return movebase(move) == 2 * moveaxis(move) ? move + 3 : move - 3;
}

STATIC_INLINE uint8_t
reorient_move(uint8_t m, uint8_t or)
{
	return transform_move(m, orientation_trans[or]);
}

STATIC_INLINE uint8_t
inverse_reorient_move(uint8_t m, uint8_t or)
{
	return transform_move(m, inverse_trans_table[orientation_trans[or]]);
}

STATIC_INLINE uint8_t
movefollow(uint8_t move)
{
	uint8_t b, m;

	if (move <= MOVE_B3)
		return move;

	if (move <= MOVE_Bw3)
		return move - MOVE_Uw;

	b = UINT8_C(3) * (move / UINT8_C(3));
	m = move - b;
	switch (b) {
	case MOVE_M:
		return MOVE_L + m;
	case MOVE_S:
		return MOVE_F + m;
	case MOVE_E:
		return MOVE_D + m;
	case MOVE_x:
		return MOVE_R + m;
	case MOVE_y:
		return MOVE_U + m;
	case MOVE_z:
		return MOVE_F + m;
	default:
		return UINT8_ERROR;
	}
}

STATIC cube_t
move(cube_t c, uint8_t m)
{
	switch (m) {
	case MOVE_U:
		return MOVE(U, c);
	case MOVE_U2:
		return MOVE(U2, c);
	case MOVE_U3:
		return MOVE(U3, c);
	case MOVE_D:
		return MOVE(D, c);
	case MOVE_D2:
		return MOVE(D2, c);
	case MOVE_D3:
		return MOVE(D3, c);
	case MOVE_R:
		return MOVE(R, c);
	case MOVE_R2:
		return MOVE(R2, c);
	case MOVE_R3:
		return MOVE(R3, c);
	case MOVE_L:
		return MOVE(L, c);
	case MOVE_L2:
		return MOVE(L2, c);
	case MOVE_L3:
		return MOVE(L3, c);
	case MOVE_F:
		return MOVE(F, c);
	case MOVE_F2:
		return MOVE(F2, c);
	case MOVE_F3:
		return MOVE(F3, c);
	case MOVE_B:
		return MOVE(B, c);
	case MOVE_B2:
		return MOVE(B2, c);
	case MOVE_B3:
		return MOVE(B3, c);
	default:
		LOG("move error: %" PRIu8 " is not a basic move\n", m);
		return ZERO_CUBE;
	}
}

STATIC uint8_t
transform_move(uint8_t m, uint8_t t)
{
	uint8_t a, base, modifier;

	if (m > MOVE_B3) {
		LOG("transform_move: attempting to transform %s, but "
		    "transofrmations are only supported for basic moves\n",
		    movestr[m]);
		return UINT8_ERROR;
	}

	a = moveaxis(m);
	base = trans_move_table[t][a];
	if (movebase(m) != 2 * a)
		base = moveopposite(base);

	modifier = m % 3;
	if (t >= TRANS_UFm)
		modifier = 2 - modifier;

	return base + modifier;
}

/* Applies the INVERSE of m BEFORE the scramble corresponding to c */
STATIC cube_t
premove(cube_t c, uint8_t m)
{
	switch (m) {
	case MOVE_U:
		return PREMOVE(U3, c);
	case MOVE_U2:
		return PREMOVE(U2, c);
	case MOVE_U3:
		return PREMOVE(U, c);
	case MOVE_D:
		return PREMOVE(D3, c);
	case MOVE_D2:
		return PREMOVE(D2, c);
	case MOVE_D3:
		return PREMOVE(D, c);
	case MOVE_R:
		return PREMOVE(R3, c);
	case MOVE_R2:
		return PREMOVE(R2, c);
	case MOVE_R3:
		return PREMOVE(R, c);
	case MOVE_L:
		return PREMOVE(L3, c);
	case MOVE_L2:
		return PREMOVE(L2, c);
	case MOVE_L3:
		return PREMOVE(L, c);
	case MOVE_F:
		return PREMOVE(F3, c);
	case MOVE_F2:
		return PREMOVE(F2, c);
	case MOVE_F3:
		return PREMOVE(F, c);
	case MOVE_B:
		return PREMOVE(B3, c);
	case MOVE_B2:
		return PREMOVE(B2, c);
	case MOVE_B3:
		return PREMOVE(B, c);
	default:
		LOG("premove error: unknown move %" PRIu8 "\n", m);
		return ZERO_CUBE;
	}
}

STATIC uint8_t
inverse_move(uint8_t m)
{
	return m - 2 * (m % 3) + 2;	
}

STATIC void
sortparallel_moves(size_t n, uint8_t *moves)
{
	uint8_t i;

	if (n < 2)
		return;

	for (i = 0; i < n-1; i++)
		if (parallel(moves[i], moves[i+1]) && moves[i] > moves[i+1])
			SWAP(moves[i], moves[i+1]);
}

STATIC bool
are_lastmoves_singlecw(size_t n, const uint8_t *moves)
{
	bool two;

	if (n == 0)
		return true;

	two = n > 1 && parallel(moves[n-1], moves[n-2]);

	return isbase(moves[n-1]) && (!two || isbase(moves[n-2]));
}
