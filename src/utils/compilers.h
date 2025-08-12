#if defined(__GNUC__)

#define unused __attribute__((unused))

#elif defined(__clang__)

#define unused __attribute__((unused))

#else

#define unused

#endif
