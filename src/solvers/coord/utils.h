STATIC coord_t *parse_coord(size_t n, const char [n]);
STATIC uint8_t parse_axis(size_t n, const char [n]);
STATIC void parse_coord_and_axis(
    size_t n, const char [n], coord_t **, uint8_t *);
STATIC long long dataid_coord(const char *, char [static NISSY_SIZE_DATAID]);

STATIC coord_t *
parse_coord(size_t n, const char coord[n])
{
	int i;

	for (i = 0; all_coordinates[i] != NULL; i++)
		if (!strncmp(all_coordinates[i]->name, coord, n))
			return all_coordinates[i];

	return NULL;
}

STATIC uint8_t
parse_axis(size_t n, const char axis[n])
{
	if (!strncmp(axis, "UD", n) || !strncmp(axis, "DU", n)) {
		return AXIS_UD;
	} else if (!strncmp(axis, "RL", n) || !strncmp(axis, "LR", n)) {
		return AXIS_RL;
	} else if (!strncmp(axis, "FB", n) || !strncmp(axis, "BF", n)) {
		return AXIS_FB;
	}

	return UINT8_ERROR;
}

STATIC void
parse_coord_and_axis(
	size_t n,
	const char str[n],
	coord_t **coord,
	uint8_t *axis
)
{
	const char *s;
	size_t i;

	s = str + 6;
	for (i = 0; i < n; i++)
		if (s[i] == '_')
			break;

	if (coord != NULL)
		*coord = parse_coord(i, s);

	if (axis != NULL)
		*axis = i == n ? UINT8_ERROR : parse_axis(n-i-1, s+i+1);
}

STATIC long long
dataid_coord(const char *ca, char dataid[static NISSY_SIZE_DATAID])
{
	coord_t *c;

	parse_coord_and_axis(strlen(ca), ca, &c, NULL);

	if (c == NULL) {
		LOG("Error: cannot parse coordinate from '%s'\n", ca);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	strcpy(dataid, c->name);

	return NISSY_OK;
}
