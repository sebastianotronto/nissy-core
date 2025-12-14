#if SIZE_MAX == UINT64_MAX
#define H48_HMAX UINT8_C(11)
#else
#define H48_HMAX UINT8_C(7)
#endif

long long parse_h48h(const char *, uint8_t [static 1]);
STATIC long long dataid_h48(const char *, char [static NISSY_SIZE_DATAID]);

long long
parse_h48h(const char *buf, uint8_t h[static 1])
{
	char format_error_msg[100];
	sprintf(format_error_msg, "[H48] Error parsing H48 solver: must be in "
	    "'h48h*' format, but got '%s'\n", buf);

	buf += 3;

	if (*buf != 'h') {
		LOG(format_error_msg);
		goto parse_h48h_error;
	}
	buf++;

	*h = atoi(buf);
	if (*h > H48_HMAX) {
		LOG("[H48] Invalid value %" PRIu8 " for parameter h (must be "
		     "at most %" PRIu8 ")\n", *h, H48_HMAX);
		goto parse_h48h_error;
	}

	for ( ; *buf >= 0 + '0' && *buf <= 9 + '0'; buf++) {
		if (*buf == 0) {
			LOG(format_error_msg);
			goto parse_h48h_error;
		}
	}

	return NISSY_OK;

parse_h48h_error:
	*h = 0;
	return NISSY_ERROR_INVALID_SOLVER;
}

STATIC long long
dataid_h48(const char *str, char buf[static NISSY_SIZE_DATAID])
{
	uint8_t h;
	long long err;

	err = parse_h48h(str, &h);
	if (err < 0)
		return err;

	sprintf(buf, "h48h%" PRIu8, h);
	return NISSY_OK;
}
