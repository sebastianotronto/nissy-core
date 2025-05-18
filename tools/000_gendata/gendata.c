#include "../tool.h"
#include "../expected_distributions.h"

char *solver;
uint64_t *expected;

static void
run(void) {
	int64_t size;
	bool consistent, expected;
	char filename[1024], dataid[NISSY_SIZE_DATAID];
	unsigned char *buf;
	
	size = generatetable(solver, &buf, dataid);
	switch (size) {
	case -1:
		return;
	case -2:
		goto gendata_run_finish;
	default:
		nissy_datainfo(size, buf);
		consistent = nissy_checkdata(size, buf) == 0;
		expected = check_distribution(solver, size, buf);
		if (consistent && expected) {
			printf("\n");
			printf("Generated %" PRId64 " bytes.\n", size);
			sprintf(filename, "tables/%s", dataid);
			writetable(buf, size, filename);
		}
		if (!consistent)
			printf("Error: table is not consistent with info"
			    " (nissy_checkdata() failed)\n");
		if (!expected)
			printf("Error: distribution is not as expected\n");
		break;
	}

gendata_run_finish:
	free(buf);
}

int main(int argc, char **argv) {
	uint8_t h, k;

	if (argc < 2) {
		printf("Error: not enough arguments. "
		    "A solver must be given.\n");
		return 1;
	}

	solver = argv[1];
	parse_h48_hk(solver, &h, &k);
	expected = expected_h48[h][k];

	nissy_setlogger(log_stderr, NULL);

	timerun(run);

	return 0;
}
