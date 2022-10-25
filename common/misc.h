#ifndef MISC_H_
#define MISC_H_


int ecall_nn(float *sample);


#define LOGSTR_SIZE 512

#define err(fmt, ...)													\
	do {																\
		char logstr[LOGSTR_SIZE];										\
		int ret = 0;													\
        printf("Error: " fmt "", ##__VA_ARGS__);				\
    } while (0)
#define warn(fmt, ...)													\
	do {																\
		char logstr[LOGSTR_SIZE];										\
		int ret = 0;													\
        printf("Warning: " fmt "", ##__VA_ARGS__);				\
    } while (0)
#define info(fmt, ...)													\
	do {																\
		char logstr[LOGSTR_SIZE];										\
		int ret = 0;													\
        printf("" fmt "", ##__VA_ARGS__);						\
    } while (0)

#ifdef DEBUG
#define dbg(fmt, ...)		\
	do {		\
		char logstr[LOGSTR_SIZE];	\
		int ret = 0;			\
        printf(":%s:%d: " fmt "\n", __func__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
#define dbg(fmt, ...) do { } while (0)
#endif

extern int g_args_debug;
extern int g_args_help;


#endif // MISC_H_
