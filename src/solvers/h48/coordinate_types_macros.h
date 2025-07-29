#define H48_ESIZE(h) ((COMB_12_4 * COMB_8_4) << (uint64_t)(h))

#define COCLASS_MASK (UINT32_C(0xFFFF) << UINT32_C(16))
#define COCLASS(x)   (((x) & COCLASS_MASK) >> UINT32_C(16))
#define ECLASS(x)    COCLASS(x)
#define TTREP_MASK   (UINT32_C(0xFF) << UINT32_C(8))
#define TTREP(x)     (((x) & TTREP_MASK) >> UINT32_C(8))
