#ifndef __DFT_H__
#define __DFT_H__

#include <stdio.h>
#include <stdint.h>
#include <istream>
#include <ostream>

#define DFT_EXPORT	extern "C"

typedef void DFT;

DFT_EXPORT int32_t DFT_getParamInt(int32_t paramId);
DFT_EXPORT char *DFT_getParamString(int32_t paramId);

DFT_EXPORT int32_t DFT_getModeParamInt(int32_t paramId, int32_t mode);
DFT_EXPORT char *DFT_getModeParamString(int32_t paramId, int32_t mode);

DFT_EXPORT DFT *DFT_init(int32_t mode, uint64_t n, uint64_t flags);
DFT_EXPORT void DFT_dispose(DFT *p, int32_t mode);

DFT_EXPORT int32_t DFT_fwrite(DFT *p, FILE *fp);
DFT_EXPORT DFT *DFT_fread(FILE *fp, int32_t *errcode);

DFT_EXPORT int32_t DFT_getPlanParamInt(int32_t paramId, void *p);

DFT_EXPORT void DFT_execute(DFT *p, int32_t mode, void *s, int32_t dir);

DFT_EXPORT uint32_t DFT_ilog2(uint32_t q);
DFT_EXPORT double DFT_timeofday(void);

DFT_EXPORT DFT *DFT_sread(std::istream & stream, int32_t *errcode);
DFT_EXPORT int32_t DFT_swrite(void *p2, std::ostream & stream);

#define DFT_FLAG_NO_TEST_RUN ( 0ULL << 0)
#define DFT_FLAG_LIGHT_TEST_RUN ( 1ULL << 0)
#define DFT_FLAG_HEAVY_TEST_RUN ( 2ULL << 0)
#define DFT_FLAG_EXHAUSTIVE_TEST_RUN ( 3ULL << 0)

#define DFT_FLAG_REAL (1ULL << 2)
#define DFT_FLAG_ALT_REAL (1ULL << 3)
#define DFT_FLAG_VERBOSE (1ULL << 4)
#define DFT_FLAG_NO_BITREVERSAL (1ULL << 5)
#define DFT_FLAG_FORCE_RECURSIVE (1ULL << 6)
#define DFT_FLAG_FORCE_COBRA (1ULL << 7)

#define DFT_PARAMID_TYPE ( 1 | ( 3 << 24 ))
#define DFT_PARAMID_MODE ( 2 | ( 3 << 24 ))
#define DFT_PARAMID_FFT_LENGTH ( 3 | ( 3 << 24 ))
#define DFT_PARAMID_IS_REAL_TRANSFORM ( 4 | ( 3 << 24 ))
#define DFT_PARAMID_IS_ALT_REAL_TRANSFORM ( 5 | ( 3 << 24 ))
#define DFT_PARAMID_NO_BIT_REVERSAL ( 6 | ( 3 << 24 ))
#define DFT_PARAMID_TEST_RUN ( 7 | ( 3 << 24 ))

#define DFT_ERROR_NOERROR 0
#define DFT_ERROR_FILE_VERSION 1
#define DFT_ERROR_FILE_IO 2
#define DFT_ERROR_UNEXPECTED_EOF 3
#define DFT_ERROR_MODE_NOT_COMPILED_IN 4
#define DFT_ERROR_MODE_NOT_AVAILABLE 5
#define DFT_ERROR_UNKNOWN_MODE 6

#endif
