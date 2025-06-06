STATIC coord_t *parse_coord(size_t, const char *);
STATIC uint8_t parse_axis(const char [static 2]);
STATIC void parse_coord_and_axis(const char *, coord_t **, uint8_t *);
STATIC long long dataid_coord(const char *, char [static NISSY_SIZE_DATAID]);

STATIC coord_t *
parse_coord(size_t n, const char *coord)
{
	int i;

	for (i = 0; all_coordinates[i] != NULL; i++)
		if (!strncmp(all_coordinates[i]->name, coord, n))
			return all_coordinates[i];

	return NULL;
}

STATIC uint8_t
parse_axis(const char axis[static 2])
{
	if (!strcmp(axis, "UD") || !strcmp(axis, "DU")) {
		return AXIS_UD;
	} else if (!strcmp(axis, "RL") || !strcmp(axis, "LR")) {
		return AXIS_RL;
	} else if (!strcmp(axis, "FB") || !strcmp(axis, "BF")) {
		return AXIS_FB;
	}

	return UINT8_ERROR;
}

STATIC void
parse_coord_and_axis(
	const char *str,
	coord_t **coord,
	uint8_t *axis
)
{
	size_t i;

	for (i = 6; i < strlen(str); i++)
		if (str[i] == '_')
			break;

	if (coord != NULL)
		*coord = parse_coord(i-6, str+6);

	if (axis != NULL)
		*axis = i == strlen(str) ? UINT8_ERROR : parse_axis(str+i+1);
}

STATIC long long
dataid_coord(const char *ca, char dataid[static NISSY_SIZE_DATAID])
{
	coord_t *c;

	parse_coord_and_axis(ca, &c, NULL);

	if (c == NULL) {
		LOG("Error: cannot parse coordinate from '%s'\n", ca);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	strcpy(dataid, c->name);

	return NISSY_OK;
}
