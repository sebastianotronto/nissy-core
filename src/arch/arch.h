#if defined(AVX2)

#include <immintrin.h>

typedef __m256i cube_t;

#if !defined(TEST_H)
#include "common.h"
#include "avx2.h"
#include "coordinates_unoptimized.h"
#endif

#elif defined(NEON)

#include <arm_neon.h>

typedef struct {
	uint8x8_t corner;
	uint8x16_t edge;
} cube_t;

#if !defined(TEST_H)
#include "common.h"
#include "neon.h"
#include "coordinates_unoptimized.h"
#endif

#else

typedef struct {
	uint8_t corner[8];
	uint8_t edge[12];
} cube_t;

#if !defined(TEST_H)
#include "common.h"
#include "portable.h"
#include "coordinates_unoptimized.h"
#endif

#endif
