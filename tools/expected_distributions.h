uint64_t expected_cocsep[21] = {
	[0] = 1,
	[1] = 6,
	[2] = 63,
	[3] = 468,
	[4] = 3068,
	[5] = 15438,
	[6] = 53814,
	[7] = 71352,
	[8] = 8784,
	[9] = 96
};

uint64_t expected_h48[12][9][21] = {
	[0] = {
		[2] = {
			[0] = 5473562,
			[1] = 34776317,
			[2] = 68566704,
			[3] = 8750867,
		},
		[4] = {
			[0] = 1,
			[1] = 1,
			[2] = 4,
			[3] = 34,
			[4] = 331,
			[5] = 3612,
			[6] = 41605,
			[7] = 474128,
			[8] = 4953846,
			[9] = 34776317,
			[10] = 68566704,
			[11] = 8749194,
			[12] = 1673,
		},
	},
	[1] = {
		[2] = {
			[0] = 6012079,
			[1] = 45822302,
			[2] = 142018732,
			[3] = 41281787,
		},
	},
	[2] = {
		[2] = {
			[0] = 6391286,
			[1] = 55494785,
			[2] = 252389935,
			[3] = 155993794,
		},
	},
	[3] = {
		[2] = {
			[0] = 6686828,
			[1] = 63867852,
			[2] = 392789689,
			[3] = 477195231,
		},
	},
	[4] = {
		[2] = {
			[0] = 77147213,
			[1] = 543379415,
			[2] = 1139570251,
			[3] = 120982321,
		},
	},
	[5] = {
		[2] = {
			[0] = 82471284,
			[1] = 687850732,
			[2] = 2345840746,
			[3] = 645995638,
		},
	},
	[6] = {
		[2] = {
			[0] = 85941099,
			[1] = 804752968,
			[2] = 4077248182,
			[3] = 2556374551,
		},
	},
	[7] = {
		[2] = {
			[0] = 88529761,
			[1] = 897323475,
			[2] = 6126260791,
			[3] = 7936519573,
		},
	},
	[8] = {
		[2] = {
			[0] = 1051579940,
			[1] = 8136021316,
			[2] = 19024479822,
			[3] = 18851861220,
		},
	},
	[9] = {
		[2] = {
			[0] = 1102038189,
			[1] = 9888265242,
			[2] = 38299375805,
			[3] = 10904855164,
		},
	},
	[10] = {
		[2] = {
			[0] = 1133240039,
			[1] = 11196285614,
			[2] = 64164702961,
			[3] = 43894840186,
		},
	},
	[11] = {
		[2] = {
			[0] = 1150763161,
			[1] = 12045845660,
			[2] = 91163433330,
			[3] = 136418095449,
		},
	},
};

uint64_t expected_eo[21] = {
	[0] = 1,
	[1] = 2,
	[2] = 25,
	[3] = 202,
	[4] = 620,
	[5] = 900,
	[6] = 285,
	[7] = 13,
};

uint64_t expected_dr[21] = {
	[0] = 1,
	[1] = 1,
	[2] = 5,
	[3] = 44,
	[4] = 487,
	[5] = 5841,
	[6] = 68364,
	[7] = 776568,
	[8] = 7950748,
	[9] = 52098876,
	[10] = 76236234,
	[11] = 3771112,
	[12] = 129,
};

uint64_t expected_dreo[21] = {
	[0] = 1,
	[1] = 1,
	[2] = 4,
	[3] = 22,
	[4] = 160,
	[5] = 1286,
	[6] = 8550,
	[7] = 42152,
	[8] = 90748,
	[9] = 33466,
	[10] = 757,
};

static bool
distribution_equal(const uint64_t *expected, const uint64_t *actual, int n)
{
	bool equal;
	int i;

	for (i = 0, equal = true; i <= n; i++) {
		if (expected[i] != actual[i]) {
			equal = false;
			printf("Wrong value for %d: expected %" PRIu64
			    ", actual %" PRIu64 "\n",
			    i, expected[i], actual[i]);
		}
	}

	return equal;
}

STATIC bool
check_table(uint64_t *exp, tableinfo_t *info)
{
	if (!distribution_equal(exp, info->distribution, info->maxvalue)) {
		printf("ERROR! Distribution is incorrect\n");
		return false;
	}

	printf("Distribution is correct\n");
	return true;
}

static bool
check_cocsep(size_t data_size, const unsigned char *data)
{
	tableinfo_t info;

	readtableinfo(data_size, data, &info);
	return distribution_equal(
	    expected_cocsep, info.distribution, info.maxvalue);
}

static bool
unknown_h48(uint8_t h, uint8_t k)
{
	if (k != 2 && k != 4)
		return true;

	if (k == 4 && h != 0)
		return true;

	return k == 2 && h > 7;
}

STATIC bool
check_distribution(
	const char *solver,
	size_t data_size,
	const unsigned char *data
)
{
	const char *str;
	tableinfo_t info = {0};

	if (!strncmp(solver, "h48", 3)) {
		readtableinfo(data_size, data, &info);
		if (!distribution_equal(
		    expected_cocsep, info.distribution, info.maxvalue)) {
			printf("ERROR! cocsep distribution is incorrect\n");
			return false;
		}
		printf("cocsep distribution is correct\n");

		readtableinfo_n(data_size, data, 2, &info);
		if (unknown_h48(info.h48h, info.bits))
			goto check_distribution_unknown;

		return check_table(expected_h48[info.h48h][info.bits], &info);
	}

	if (!strncmp(solver, "coord_", 6)) {
		readtableinfo(data_size, data, &info);
		if (!strncmp(info.solver, "coord helper table for ", 23))
			readtableinfo_n(data_size, data, 2, &info);

		str = info.solver + 22; /* "coordinate solver for COORD" */
		if (!strcmp(str, "EO")) {
			return check_table(expected_eo, &info);
		} else if (!strcmp(str, "DR")) {
			return check_table(expected_dr, &info);
		} else if (!strcmp(str, "DREO")) {
			return check_table(expected_dreo, &info);
		} else {
			goto check_distribution_unknown;
		}
	}

check_distribution_unknown:
	printf("Distribution unknown, not checked\n");
	return true;
}
