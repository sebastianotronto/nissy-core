#if THREADS == 1

#define wrapthread_atomic

#define wrapthread_define_var_thread_t(x) unused char x
#define wrapthread_define_var_mutex_t(x) unused char x
#define wrapthread_define_struct_thread_t(x) char x
#define wrapthread_define_struct_mutex_t(x) char x

#define wrapthread_define_if_threads(T, x) T x; (void)(x)

#define wrapthread_create(a, b, c, d) c(d)
#define wrapthread_join(a, b)
#define wrapthread_mutex_init(a, b)
#define wrapthread_mutex_lock(a)
#define wrapthread_mutex_unlock(a)

#else

#include <pthread.h>

#define wrapthread_atomic _Atomic

#define wrapthread_define_var_thread_t(x) pthread_t x
#define wrapthread_define_var_mutex_t(x) pthread_mutex_t x
#define wrapthread_define_struct_thread_t(x) pthread_t x
#define wrapthread_define_struct_mutex_t(x) pthread_mutex_t x

#define wrapthread_define_if_threads(T, x) T x

#define wrapthread_create(a, b, c, d) pthread_create(a, b, c, d)
#define wrapthread_join(a, b) pthread_join(a, b)
#define wrapthread_mutex_init(a, b) pthread_mutex_init(a, b)
#define wrapthread_mutex_lock(a) pthread_mutex_lock(a)
#define wrapthread_mutex_unlock(a) pthread_mutex_unlock(a)

#endif
