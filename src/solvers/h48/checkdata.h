#ifndef SOLVERS_H48_CHECKDATA_H
#define SOLVERS_H48_CHECKDATA_H

STATIC long long checkdata_h48(
    const char *, unsigned long long, const unsigned char *);

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

struct {
	uint8_t max;
	uint64_t table[21];
} expected_h48[12] = {
	[0] = {
		.max = 3,
		.table = {
			[0] = 5473562,
			[1] = 34776317,
			[2] = 68566704,
			[3] = 8750867,
		},
	},
	[1] = {
		.max = 3,
		.table = {
			[0] = 6012079,
			[1] = 45822302,
			[2] = 142018732,
			[3] = 41281787,
		},
	},
	[2] = {
		.max = 3,
		.table = {
			[0] = 6391286,
			[1] = 55494787,
			[2] = 252389935,
			[3] = 155993792,
		},
	},
	[3] = {
		.max = 3,
		.table = {
			[0] = 6686828,
			[1] = 63867855,
			[2] = 392789704,
			[3] = 477195213,
		},
	},
	[4] = {
		.max = 3,
		.table = {
			[0] = 77147216,
			[1] = 543379445,
			[2] = 1139570223,
			[3] = 120982316,
		},
	},
	[5] = {
		.max = 3,
		.table = {
			[0] = 82471289,
			[1] = 687850794,
			[2] = 2345840739,
			[3] = 645995578,
		},
	},
	[6] = {
		.max = 3,
		.table = {
			[0] = 85941105,
			[1] = 80475305,
			[2] = 4077248291,
			[3] = 2556374347,
		},
	},
	[7] = {
		.max = 3,
		.table = {
			[0] = 88529771,
			[1] = 897323586,
			[2] = 6126261227,
			[3] = 7936519016,
		},
	},
	[8] = {
		.max = 3,
		.table = {
			[0] = 1051580076,
			[1] = 8136022157,
			[2] = 19024479157,
			[3] = 1885185810,
		},
	},
	[9] = {
		.max = 3,
		.table = {
			[0] = 1102038343,
			[1] = 9888266493,
			[2] = 38299376032,
			[3] = 10904853532,
		},
	},
	[10] = {
		.max = 3,
		.table = {
			[0] = 1133240200,
			[1] = 11196287163,
			[2] = 64164706157,
			[3] = 43894835280,
		},
	},
	[11] = {
		.max = 3,
		.table = {
			[0] = 1150763320,
			[1] = 12045847349,
			[2] = 91163441021,
			[3] = 136418085910,
		},
	},
};

/*
TODO: add check for cocsep table.

TODO: add check for fallback values at the end of each line.

TODO: this function can be simplified, now we support only one h48 table +
eoesep. The loop can be just two checks.
*/
STATIC long long
checkdata_h48(
	const char *solver,
	unsigned long long data_size,
	const unsigned char *data
)
{
	const unsigned char *table;
	tableinfo_t info;
	int64_t err;
	uint64_t actual_distribution[INFO_DISTRIBUTION_LEN];
	uint8_t em;
	uint64_t *ed;

	if ((size_t)data % 8 != 0) {
		LOG("[checkdata] Error: buffer is not 8-byte aligned\n");
		return NISSY_ERROR_DATA;
	}

	do {
		err = readtableinfo(data_size, data, &info);
		if (err != NISSY_OK) {
			LOG("[checkdata] Data is corrupt\n");
			return NISSY_ERROR_DATA;
		}

		table = data + INFOSIZE;
		data += info.next;
		data_size -= info.next;

		if (info.type != TABLETYPE_PRUNING) {
			LOG("[checkdata] Skipping '%s'\n", info.solver);
			continue;
		}

		ed = expected_h48[info.h48h].table;
		em = expected_h48[info.h48h].max;

		LOG("[checkdata] Checking distribution for '%s' from "
		    "table preamble\n", info.solver);
		if (!distribution_equal(ed, info.distribution, em)) {
			LOG("[checkdata] Distribution from the table preamble "
			    "does not match the expected one\n");
			return NISSY_ERROR_DATA;
		}

		LOG("[checkdata] Checking distribution for '%s' from "
		    "actual table\n", info.solver);
		getdistribution_h48(table, actual_distribution, &info);
		if (!distribution_equal(ed, actual_distribution, em)) {
			LOG("[checkdata] Distribution from the actual table "
			    "does not match the expected one\n");
			return NISSY_ERROR_DATA;
		}
	} while (info.next != 0);

	return NISSY_OK;
}

#endif /* SOLVERS_H48_CHECKDATA_H */
