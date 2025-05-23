#define ENTRIES_PER_BYTE(k) (UINT64_C(8) / (uint64_t)(k))
#define TABLE_SHIFT(i, k)   ((uint8_t)(k) * (uint8_t)((i) % ENTRIES_PER_BYTE(k)))
#define TABLE_MASK(i, k)    ((UINT8_BIT(k) - UINT8_C(1)) << TABLE_SHIFT(i, k))

typedef struct {
	uint64_t min;
	uint64_t max;
	uint8_t bits;
	uint64_t *distr;
	const unsigned char *table;
} getdistribution_data_t;

STATIC void *getdistribution_runthread(void *);
STATIC void getdistribution(const unsigned char *,
    uint64_t [static INFO_DISTRIBUTION_LEN], const tableinfo_t [static 1]);
STATIC bool distribution_equal(const uint64_t [static INFO_DISTRIBUTION_LEN],
    const uint64_t [static INFO_DISTRIBUTION_LEN], uint8_t);

STATIC void *
getdistribution_runthread(void *arg)
{
	getdistribution_data_t *data = (getdistribution_data_t *)arg;
	const unsigned char *table;
	uint8_t j, k, m;
	uint64_t i;

	memset(data->distr, 0, INFO_DISTRIBUTION_LEN * sizeof(uint64_t));

	k = data->bits;
	table = data->table;
	m = TABLE_MASK(0, k);
	for (i = data->min; i < data->max; i++)
		for (j = 0; j < ENTRIES_PER_BYTE(k); j++)
			data->distr[(table[i] & (m << (j*k))) >> (j*k)]++;

	return NULL;
}

STATIC void
getdistribution(
	const unsigned char *table,
	uint64_t distr[static INFO_DISTRIBUTION_LEN],
	const tableinfo_t info[static 1]
) {
	getdistribution_data_t targ[THREADS];
	pthread_t thread[THREADS];
	uint8_t pval, k;
	uint64_t local_distr[THREADS][INFO_DISTRIBUTION_LEN];
	uint64_t i, j, nbytes, sz, epb;

	k = info->bits;
	epb = ENTRIES_PER_BYTE(k);
	nbytes = info->entries / epb;
	sz = nbytes / THREADS;
	for (i = 0; i < THREADS; i++) {
		targ[i] = (getdistribution_data_t) {
			.min = i * sz,
			.max = i == THREADS - 1 ? nbytes : (i+1) * sz,
			.bits = k,
			.distr = local_distr[i],
			.table = table,
		};
		pthread_create(&thread[i], NULL,
		    getdistribution_runthread, &targ[i]);
	}

	for (i = 0; i < THREADS; i++)
		pthread_join(thread[i], NULL);

	memset(distr, 0, INFO_DISTRIBUTION_LEN * sizeof(uint64_t));
	for (i = 0; i < THREADS; i++)
		for (j = 0; j < INFO_DISTRIBUTION_LEN; j++)
			distr[j] += local_distr[i][j];

	for (i = nbytes * epb; i < info->entries; i++) {
		pval = (table[i/epb] & TABLE_MASK(i, k)) >> TABLE_SHIFT(i, k);
		distr[pval]++;
	}
}

STATIC bool
distribution_equal(
	const uint64_t expected[static INFO_DISTRIBUTION_LEN],
	const uint64_t actual[static INFO_DISTRIBUTION_LEN],
	uint8_t maxvalue
)
{
	int wrong;
	uint8_t i;

	for (i = 0, wrong = 0; i <= MIN(maxvalue, 20); i++) {
		if (expected[i] != actual[i]) {
			wrong++;
			LOG("[checkdata] Value for depth %" PRIu8
			    ": expected %" PRIu64 ", found %" PRIu64 "\n",
			    i, expected[i], actual[i]);
		}
		/*
		else {
			LOG("[checkdata] Value for depth %" PRIu8
			    " is correct (%" PRIu64 ")\n", i, actual[i]);
		}
		*/
	}

	return wrong == 0;
}
