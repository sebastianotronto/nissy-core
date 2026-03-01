#if THREADS == 1
#define WRAPTHREADS_NOTHREADS 1
#endif

#if !WRAPTHREADS_NOTHREADS

	#if defined(__unix__)

		#include <threads.h>

		#define wrapthread_atomic _Atomic

		#define wrapthread_return_t int
		#define wrapthread_return_val 0

		#define wrapthread_define_var_thread_t(x) thrd_t x
		#define wrapthread_define_var_mutex_t(x) mtx_t x
		#define wrapthread_define_struct_thread_t(x) thrd_t x
		#define wrapthread_define_struct_mutex_t(x) mtx_t x

		#define wrapthread_define_if_threads(T, x) T x

		#define wrapthread_create(a, f, arg) thrd_create(a, f, arg)
		#define wrapthread_join(a) thrd_join(a, NULL)
		#define wrapthread_mutex_init(a) mtx_init(a, mtx_plain)
		#define wrapthread_mutex_lock(a) mtx_lock(a)
		#define wrapthread_mutex_unlock(a) mtx_unlock(a)

	#elif !defined(__STDC_NO_THREADS__)

		#include <pthread.h>

		#define wrapthread_atomic _Atomic

		#define wrapthread_return_t void *
		#define wrapthread_return_val NULL

		#define wrapthread_define_var_thread_t(x) pthread_t x
		#define wrapthread_define_var_mutex_t(x) pthread_mutex_t x
		#define wrapthread_define_struct_thread_t(x) pthread_t x
		#define wrapthread_define_struct_mutex_t(x) pthread_mutex_t x

		#define wrapthread_define_if_threads(T, x) T x

		#define wrapthread_create(a, f, arg) \
		    pthread_create(a, NULL, f, arg)
		#define wrapthread_join(a) pthread_join(a, NULL)
		#define wrapthread_mutex_init(a) pthread_mutex_init(a, NULL)
		#define wrapthread_mutex_lock(a) pthread_mutex_lock(a)
		#define wrapthread_mutex_unlock(a) pthread_mutex_unlock(a)

	#else
		#define WRAPTHREADS_NOTHREADS 1
	#endif

#endif

#if WRAPTHREADS_NOTHREADS

	#define wrapthread_atomic

	#define wrapthread_return_t int
	#define wrapthread_return_val 0

	#define wrapthread_define_var_thread_t(x) unused char x
	#define wrapthread_define_var_mutex_t(x) unused char x
	#define wrapthread_define_struct_thread_t(x) char x
	#define wrapthread_define_struct_mutex_t(x) char x

	#define wrapthread_define_if_threads(T, x) T x; (void)(x)

	#define wrapthread_create(t, f, arg) f(arg)
	#define wrapthread_join(a)
	#define wrapthread_mutex_init(a)
	#define wrapthread_mutex_lock(a)
	#define wrapthread_mutex_unlock(a)

#endif
