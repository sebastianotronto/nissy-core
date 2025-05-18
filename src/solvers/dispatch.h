typedef struct {
	const char *prefix;
	long long (*dataid)(const char *, char [static NISSY_SIZE_DATAID]);
	long long (*gendata)(
	    const char *, unsigned long long, unsigned char *);
	long long (*solve)(oriented_cube_t, const char *, unsigned, unsigned,
	    unsigned, unsigned, unsigned, unsigned, unsigned long long,
	    const unsigned char *, unsigned, char *,
	    long long [static NISSY_SIZE_SOLVE_STATS],
	    int (*)(void *), void *);
} solver_dispatch_t;

STATIC solver_dispatch_t *match_solver(const char *);

solver_dispatch_t solver_dispatchers[] = {
{
	.prefix = "h48",
	.dataid = dataid_h48,
	.gendata = gendata_h48_dispatch,
	.solve = solve_h48_dispatch,
},
{
	.prefix = "coord_",
	.dataid = dataid_coord,
	.gendata = gendata_coord_dispatch,
	.solve = solve_coord_dispatch,
},
{
	.prefix = NULL
}
};

STATIC solver_dispatch_t *
match_solver(const char *name)
{
	const char *prefix;
	int i;

	if (name == NULL)
		return NULL;

	for (i = 0; solver_dispatchers[i].prefix != NULL; i++) {
		prefix = solver_dispatchers[i].prefix;
		if (!strncmp(name, prefix, strlen(prefix)))
			return &solver_dispatchers[i];
	}

	return NULL;
}
