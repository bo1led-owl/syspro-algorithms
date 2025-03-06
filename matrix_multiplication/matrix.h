#pragma once

#include <stddef.h>

typedef int MatT;
typedef void (*MulFn)(size_t, MatT*, MatT*, MatT*);

#ifdef __cplusplus
extern "C" {
#endif

void mulNaive(size_t n, MatT* buf, MatT* lhs, MatT* rhs);
void mulBad(size_t n, MatT* buf, MatT* lhs, MatT* rhs);
void mulStrassen(size_t n, MatT* buf, MatT* lhs, MatT* rhs);

#ifdef __cplusplus
}
#endif
