STATIC long long checkdata_h48(
    const char *, unsigned long long, const unsigned char *);

/*
Currently unused.
TODO: re-introduce check on cocsep table
*/
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
} expected_h48[12][9] = {
	[0] = {
		[2] = {
			.max = 3,
			.table = {
				[0] = 5473562,
				[1] = 34776317,
				[2] = 68566704,
				[3] = 8750867,
			},
		},
		[4] = {
			.max = 12,
			.table = {
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
	},
	[1] = {
		[2] = {
			.max = 3,
			.table = {
				[0] = 6012079,
				[1] = 45822302,
				[2] = 142018732,
				[3] = 41281787,
			},
		},
	},
	[2] = {
		[2] = {
			.max = 3,
			.table = {
				[0] = 6391286,
				[1] = 55494785,
				[2] = 252389935,
				[3] = 155993794,
			},
		},
	},
	[3] = {
		[2] = {
			.max = 3,
			.table = {
				[0] = 6686828,
				[1] = 63867852,
				[2] = 392789689,
				[3] = 477195231,
			},
		},
	},
	[4] = {
		[2] = {
			.max = 3,
			.table = {
				[0] = 77147213,
				[1] = 543379415,
				[2] = 1139570251,
				[3] = 120982321,
			},
		},
	},
	[5] = {
		[2] = {
			.max = 3,
			.table = {
				[0] = 82471284,
				[1] = 687850732,
				[2] = 2345840746,
				[3] = 645995638,
			},
		},
	},
	[6] = {
		[2] = {
			.max = 3,
			.table = {
				[0] = 85941099,
				[1] = 804752968,
				[2] = 4077248182,
				[3] = 2556374551,
			},
		},
	},
	[7] = {
		[2] = {
			.max = 3,
			.table = {
				[0] = 88529761,
				[1] = 897323475,
				[2] = 6126260791,
				[3] = 7936519573,
			},
		},
	},
	[8] = {
		[2] = {
			.max = 3,
			.table = {
				[0] = 1051579940,
				[1] = 8136021316,
				[2] = 19024479822,
				[3] = 18851861220,
			},
		},
	},
	[9] = {
		[2] = {
			.max = 3,
			.table = {
				[0] = 1102038189,
				[1] = 9888265242,
				[2] = 38299375805,
				[3] = 10904855164,
			},
		},
	},
	[10] = {
		[2] = {
			.max = 3,
			.table = {
				[0] = 1133240039,
				[1] = 11196285614,
				[2] = 64164702961,
				[3] = 43894840186,
			},
		},
	},
	[11] = {
		[2] = {
			.max = 3,
			.table = {
				[0] = 1150763161,
				[1] = 12045845660,
				[2] = 91163433330,
				[3] = 136418095449,
			},
		},
	},
};

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

		ed = expected_h48[info.h48h][info.bits].table;
		em = expected_h48[info.h48h][info.bits].max;

		LOG("[checkdata] Checking distribution for '%s' from "
		    "table preamble\n", info.solver);
		if (!distribution_equal(ed, info.distribution, em)) {
			LOG("[checkdata] Distribution from the table preamble "
			    "does not matche the expected one\n");
			return NISSY_ERROR_DATA;
		}

		LOG("[checkdata] Checking distribution for '%s' from "
		    "actual table\n", info.solver);
		getdistribution(table, actual_distribution, &info);
		if (!distribution_equal(ed, actual_distribution, em)) {
			LOG("[checkdata] Distribution from the actual table "
			    "does not match the expected one\n");
			return NISSY_ERROR_DATA;
		}
	} while (info.next != 0);

	return NISSY_OK;
}
