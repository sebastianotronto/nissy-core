STATIC coord_t *parse_coord(size_t, const char *);
STATIC void parse_coord_and_trans(const char *, coord_t **, uint8_t *);
STATIC long long dataid_coord(const char *, char [static NISSY_SIZE_DATAID]);

STATIC coord_t *
parse_coord(size_t n, const char *coord)
{
	int i;

	/*
	TODO Some coordinates are parsed incorrectly, e.g. DRFINNOE
        TODO is matched by DRFIN. Check that strncmp is used correctly.
	*/
	for (i = 0; all_coordinates[i] != NULL; i++)
		if (!strncmp(all_coordinates[i]->name, coord, n))
			return all_coordinates[i];

	return NULL;
}

STATIC void
parse_coord_and_trans(
	const char *str,
	coord_t **coord,
	uint8_t *trans
)
{
	size_t i;

	for (i = 6; i < strlen(str); i++)
		if (str[i] == '_')
			break;

	if (coord != NULL)
		*coord = parse_coord(i-6, str+6);

	if (trans != NULL)
		*trans = i == strlen(str) ?
		    UINT8_ERROR : readrotation(str+i+1);
}

STATIC long long
dataid_coord(const char *ca, char dataid[static NISSY_SIZE_DATAID])
{
	coord_t *c;

	parse_coord_and_trans(ca, &c, NULL);

	if (c == NULL) {
		LOG("Error: cannot parse coordinate from '%s'\n", ca);
		return NISSY_ERROR_INVALID_SOLVER;
	}

	strcpy(dataid, c->name);

	return NISSY_OK;
}
