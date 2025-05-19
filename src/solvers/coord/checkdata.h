STATIC long long checkdata_coord(
    const char *, unsigned long long n, const unsigned char [n]);

STATIC long long
checkdata_coord(
	const char *solver,
	unsigned long long data_size,
	const unsigned char data[data_size]
)
{
	coord_t *coord;
	const unsigned char *table;
	tableinfo_t info;
	int64_t err;
	uint64_t actual_distribution[INFO_DISTRIBUTION_LEN];

	if ((size_t)data % 8 != 0) {
		LOG("[checkdata] Error: buffer is not 8-byte aligned\n");
		return NISSY_ERROR_DATA;
	}

	parse_coord_and_axis(solver, &coord, NULL);
	if (coord == NULL) {
		LOG("[cehckdata] Unknown coordinate solver '%s'\n", solver);
		return NISSY_ERROR_DATA;
	}

	table = data + INFOSIZE;
	err = readtableinfo(data_size, data, &info);
	if (err != NISSY_OK) {
		LOG("[checkdata] Data is corrupt\n");
		return err;
	}

	if (info.type != TABLETYPE_PRUNING) {
		LOG("[checkdata] Skipping '%s'\n", info.solver);
		table += info.next;
		err = readtableinfo_n(data_size, data, 2, &info);
		if (err != NISSY_OK) {
			LOG("[checkdata] Data is corrupt\n");
			return err;
		}
	}
	
	LOG("[checkdata] Checking distribution for '%s' from "
	    "table preamble\n", info.solver);
	if (!distribution_equal(coord->pruning_distribution,
	    info.distribution, coord->pruning_max)) {
		LOG("[checkdata] Distribution from the table preamble does "
		    "not match the expected one\n");
		return NISSY_ERROR_DATA;
	}

	LOG("\n[checkdata] Checking distribution for '%s' from "
	    "actual table\n", info.solver);
	getdistribution(table, actual_distribution, &info);
	if (!distribution_equal(coord->pruning_distribution,
	    actual_distribution, coord->pruning_max)) {
		LOG("[checkdata] Distribution from the actual table does "
		    "not match the expected one\n");
		return NISSY_ERROR_DATA;
	}

	return NISSY_OK;
}
