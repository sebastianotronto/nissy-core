#include "tool.h"

#define SOL_BUFFER_LEN 100000

char *solver;
int64_t size = 0;
unsigned char *buf;

bool check_one(char *actual, char *expected) {
	unsigned i;
	size_t l_actual, l_expected;

	for (l_actual = 0; actual[l_actual] != '\n'; l_actual++) ;
	l_expected = strlen(expected);
	if (l_actual > l_expected)
		return false;
	for (i = 0; i < l_expected; i++) {
		if (!strncmp(actual, &expected[i], l_actual))
			return true;
		while(expected[i] != '\n') i++;
	}
	return false;
}

bool check_all(char *actual, char *expected) {
	unsigned i, found, n_expected;
	size_t l_actual;

	l_actual = strlen(actual);
	if (l_actual != strlen(expected))
		return false;

	for (i = 0, n_expected = 0; expected[i]; i++)
		n_expected += expected[i] == '\n';

	for (i = 0, found = 0; i < l_actual; i++)
		if (i == 0 || actual[i-1] == '\n')
			found += check_one(&actual[i], expected);

	return found == n_expected;
}

void run(void) {
	int i;
	int64_t n;
	long long stats[NISSY_SIZE_SOLVE_STATS];
	char sol[SOL_BUFFER_LEN], cube[NISSY_SIZE_CUBE];

	for (i = 0; s[i].scramble[0]; i++) {
		printf("\n%d. %s\n", i, s[i].scramble);

		/* Multiple solutions */
		if (nissy_applymoves(NISSY_SOLVED_CUBE, s[i].scramble, cube)
		     == -1) {
			printf("Invalid scramble\n");
			continue;
		}
		n = nissy_solve(cube, solver,
		    NISSFLAG, MINMOVES, MAXMOVES, MAXSOLUTIONS, OPTIMAL,
		    0, size, buf, SOL_BUFFER_LEN, sol, stats,
		    NULL, NULL);
		if (check_all(sol, s[i].solutions)) {
			printf("All solutions are correct\n");
		} else {
			printf("Error!\n");
			printf("Found solution(s):\n%s", sol);
			printf("Valid solution(s):\n%s", s[i].solutions);
			return;
		}
	}

	printf("\nAll scrambles solved correctly\n");
}

int main(int argc, char **argv) {
	char filename[255], dataid[NISSY_SIZE_DATAID];

	if (argc < 2) {
		printf("Error: not enough arguments. "
		    "A solver must be given.\n");
		return 1;
	}

	solver = argv[1];
	srand(time(NULL));
	nissy_setlogger(log_stderr, NULL);

	sprintf(filename, "tables/%s", solver);
	if (getdata(solver, &buf, filename) != 0)
		return 1;

	size = nissy_solverinfo(solver, dataid);

	timerun(run);

	free(buf);
	return 0;
}
