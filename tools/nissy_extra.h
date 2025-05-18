/*
This header file exposes certain functions that are meant to be used
for testing purposes only.
*/

#define STATIC static
#define LOG printf

#include "../src/solvers/tables_types_macros.h"
#include "../src/solvers/tables.h"

size_t gendata_h48_derive(uint8_t, const unsigned char *, unsigned char *);
long long parse_h48_hk(const char *, uint8_t [static 1], uint8_t [static 1]);
long long int nissy_datainfo(uint64_t, const unsigned char *);
