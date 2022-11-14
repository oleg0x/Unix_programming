#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>



#define CHECK_ERROR(x) {						\
	int __ce_val = (x);							\
	if ( __ce_val == -1 && errno != 0 ) {		\
		fprintf(stderr, "Error in function '%s'\n%s:%d : %s (%d)\n",	\
			__func__, __FILE__, __LINE__, strerror(errno), errno);		\
		exit(EXIT_FAILURE);						\
	}											\
}



#define EC_CMP(var, err_ret) {					\
	if ( (long)(var) == (long)(err_ret) ) {		\
		fprintf(stderr, "Error in function '%s'\n%s:%d : %s (%d)\n",	\
			__func__, __FILE__, __LINE__, strerror(errno), errno);		\
		goto ec_cleanup_begin;					\
	}											\
}

#define EC_NEG1(x) EC_CMP(x, -1)

#define EC_NULL(x) EC_CMP(x, NULL)

#define EC_FALSE(x) EC_CMP(x, false)

#define EC_EOF(x) EC_CMP(x, EOF)

#define EC_NZERO(x) { if ( (x) != 0 )  EC_CMP(0, 0) }

#define EC_FAIL EC_CMP(0, 0)

#define EC_CLEANUP goto ec_cleanup_begin;

#define EC_CLEANUP_BEGIN {						\
	fprintf(stderr, "Warning: control flowed into EC_CLEANUP_BEGIN\n");	\
	ec_cleanup_begin:

#define EC_CLEANUP_END							\
	}
