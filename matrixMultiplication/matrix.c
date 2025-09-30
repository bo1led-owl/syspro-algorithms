#include "matrix.h"

#include <assert.h>
#include <immintrin.h>
#include <stdlib.h>
#include <strings.h>

void mulNaive(size_t n, MatT* buf, MatT* lhs, MatT* rhs) {
    bzero(buf, n * n * sizeof(MatT));

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            for (size_t k = 0; k < n; ++k) {
                buf[i * n + j] += lhs[i * n + k] * rhs[k * n + j];
            }
        }
    }
}

typedef struct {
    MatT* mat;
    size_t n;  // original matrix dim
    size_t m;  // submatrix dim
    size_t hor_offset;
    size_t vert_offset;
} Submatrix;

#define SUBM(mat_, n_, m_, h, v)                                                   \
    (Submatrix) {                                                                  \
        .mat = (mat_), .n = (n_), .m = (m_), .hor_offset = (h), .vert_offset = (v) \
    }

#define SUBM_OFFSET(sm, m_, h, v)                                                     \
    (Submatrix) {                                                                     \
        .mat = (sm).mat, .n = (sm).n, .m = (m_), .hor_offset = (sm).hor_offset + (h), \
        .vert_offset = (sm).vert_offset + (v)                                         \
    }

static inline MatT* getPtr(Submatrix sm, size_t i, size_t j) {
    return &sm.mat[sm.n * (sm.vert_offset + i) + sm.hor_offset + j];
}

static void add(Submatrix buf, Submatrix lhs, Submatrix rhs) {
    assert(buf.m == lhs.m);
    assert(lhs.m == rhs.m);
    assert(__builtin_popcount(buf.m) == 1);

    const size_t m = buf.m;
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < m; j += 8) {
            MatT* b = getPtr(buf, i, j);
            MatT* l = getPtr(lhs, i, j);
            MatT* r = getPtr(rhs, i, j);
            _mm256_storeu_si256((__m256i*)b, _mm256_add_epi32(_mm256_loadu_si256((__m256i*)l),
                                                              _mm256_loadu_si256((__m256i*)r)));
        }
    }
}

static void sub(Submatrix buf, Submatrix lhs, Submatrix rhs) {
    assert(buf.m == lhs.m);
    assert(lhs.m == rhs.m);
    assert(__builtin_popcount(buf.m) == 1);

    const size_t m = buf.m;
    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < m; j += 8) {
            MatT* b = getPtr(buf, i, j);
            MatT* l = getPtr(lhs, i, j);
            MatT* r = getPtr(rhs, i, j);
            _mm256_storeu_si256((__m256i*)b, _mm256_sub_epi32(_mm256_loadu_si256((__m256i*)l),
                                                              _mm256_loadu_si256((__m256i*)r)));
        }
    }
}

static void mulBadInternal(Submatrix buf, Submatrix lhs, Submatrix rhs) {
    assert(buf.m == lhs.m);
    assert(lhs.m == rhs.m);
    assert(__builtin_popcount(buf.m) == 1);

    if (buf.m <= 64) {
        const size_t m = buf.m;
        for (size_t i = 0; i < m; ++i) {
            for (size_t j = 0; j < m; ++j) {
                MatT* b = getPtr(buf, i, j);
                *b = 0;
                for (size_t k = 0; k < m; ++k) {
                    MatT* l = getPtr(lhs, i, k);
                    MatT* r = getPtr(rhs, k, j);
                    *b += (*l) * (*r);
                }
            }
        }
        return;
    }

    /*
    lhs:
        A B
        C D
    rhs:
        E F
        G H
    */

    Submatrix a = SUBM_OFFSET(lhs, lhs.m / 2, 0, 0);
    Submatrix b = SUBM_OFFSET(lhs, lhs.m / 2, lhs.m / 2, 0);
    Submatrix c = SUBM_OFFSET(lhs, lhs.m / 2, 0, lhs.m / 2);
    Submatrix d = SUBM_OFFSET(lhs, lhs.m / 2, lhs.m / 2, lhs.m / 2);

    Submatrix e = SUBM_OFFSET(rhs, rhs.m / 2, 0, 0);
    Submatrix f = SUBM_OFFSET(rhs, rhs.m / 2, rhs.m / 2, 0);
    Submatrix g = SUBM_OFFSET(rhs, rhs.m / 2, 0, rhs.m / 2);
    Submatrix h = SUBM_OFFSET(rhs, rhs.m / 2, rhs.m / 2, rhs.m / 2);

    Submatrix topleft = SUBM_OFFSET(buf, buf.m / 2, 0, 0);
    Submatrix topright = SUBM_OFFSET(buf, buf.m / 2, buf.m / 2, 0);
    Submatrix botleft = SUBM_OFFSET(buf, buf.m / 2, 0, buf.m / 2);
    Submatrix botright = SUBM_OFFSET(buf, buf.m / 2, buf.m / 2, buf.m / 2);

    Submatrix buf1 = SUBM(malloc(a.m * a.m * sizeof(MatT)), a.m, a.m, 0, 0);
    Submatrix buf2 = SUBM(malloc(a.m * a.m * sizeof(MatT)), a.m, a.m, 0, 0);

    mulBadInternal(buf1, a, e);
    mulBadInternal(buf2, b, g);
    add(topleft, buf1, buf2);

    mulBadInternal(buf1, a, f);
    mulBadInternal(buf2, b, h);
    add(topright, buf1, buf2);

    mulBadInternal(buf1, c, e);
    mulBadInternal(buf2, d, g);
    add(botleft, buf1, buf2);

    mulBadInternal(buf1, c, f);
    mulBadInternal(buf2, d, h);
    add(botright, buf1, buf2);

    free(buf1.mat);
    free(buf2.mat);
}

void mulBad(size_t n, MatT* buf, MatT* lhs, MatT* rhs) {
    assert(__builtin_popcount(n) == 1);

    mulBadInternal(SUBM(buf, n, n, 0, 0), SUBM(lhs, n, n, 0, 0), SUBM(rhs, n, n, 0, 0));
}

static void mulStrassenInternal(Submatrix buf, Submatrix lhs, Submatrix rhs) {
    assert(buf.m == lhs.m);
    assert(lhs.m == rhs.m);
    assert(__builtin_popcount(buf.m) == 1);

    if (buf.m <= 64) {
        const size_t m = buf.m;
        for (size_t i = 0; i < m; ++i) {
            for (size_t j = 0; j < m; ++j) {
                MatT* b = getPtr(buf, i, j);
                *b = 0;
                for (size_t k = 0; k < m; ++k) {
                    MatT* l = getPtr(lhs, i, k);
                    MatT* r = getPtr(rhs, k, j);
                    *b += (*l) * (*r);
                }
            }
        }
        return;
    }

    /*
    lhs:
        A B
        C D
    rhs:
        E F
        G H
    */

    Submatrix a = SUBM_OFFSET(lhs, lhs.m / 2, 0, 0);
    Submatrix b = SUBM_OFFSET(lhs, lhs.m / 2, lhs.m / 2, 0);
    Submatrix c = SUBM_OFFSET(lhs, lhs.m / 2, 0, lhs.m / 2);
    Submatrix d = SUBM_OFFSET(lhs, lhs.m / 2, lhs.m / 2, lhs.m / 2);

    Submatrix e = SUBM_OFFSET(rhs, rhs.m / 2, 0, 0);
    Submatrix f = SUBM_OFFSET(rhs, rhs.m / 2, rhs.m / 2, 0);
    Submatrix g = SUBM_OFFSET(rhs, rhs.m / 2, 0, rhs.m / 2);
    Submatrix h = SUBM_OFFSET(rhs, rhs.m / 2, rhs.m / 2, rhs.m / 2);

    Submatrix topleft = SUBM_OFFSET(buf, buf.m / 2, 0, 0);
    Submatrix topright = SUBM_OFFSET(buf, buf.m / 2, buf.m / 2, 0);
    Submatrix botleft = SUBM_OFFSET(buf, buf.m / 2, 0, buf.m / 2);
    Submatrix botright = SUBM_OFFSET(buf, buf.m / 2, buf.m / 2, buf.m / 2);

    Submatrix p1 = SUBM(malloc(a.m * a.m * sizeof(MatT)), a.m, a.m, 0, 0);
    Submatrix p2 = SUBM(malloc(a.m * a.m * sizeof(MatT)), a.m, a.m, 0, 0);
    Submatrix p3 = SUBM(malloc(a.m * a.m * sizeof(MatT)), a.m, a.m, 0, 0);
    Submatrix p4 = SUBM(malloc(a.m * a.m * sizeof(MatT)), a.m, a.m, 0, 0);
    Submatrix p5 = SUBM(malloc(a.m * a.m * sizeof(MatT)), a.m, a.m, 0, 0);
    Submatrix p6 = SUBM(malloc(a.m * a.m * sizeof(MatT)), a.m, a.m, 0, 0);
    Submatrix p7 = SUBM(malloc(a.m * a.m * sizeof(MatT)), a.m, a.m, 0, 0);

    Submatrix buf1 = SUBM(malloc(a.m * a.m * sizeof(MatT)), a.m, a.m, 0, 0);
    Submatrix buf2 = SUBM(malloc(a.m * a.m * sizeof(MatT)), a.m, a.m, 0, 0);

    // p1
    sub(buf1, f, h);
    mulStrassenInternal(p1, a, buf1);

    // p2
    add(buf1, a, b);
    mulStrassenInternal(p2, buf1, h);

    // p3
    add(buf1, c, d);
    mulStrassenInternal(p3, buf1, e);

    // p4
    sub(buf1, g, e);
    mulStrassenInternal(p4, d, buf1);

    // p5
    add(buf1, a, d);
    add(buf2, e, h);
    mulStrassenInternal(p5, buf1, buf2);

    // p6
    sub(buf1, b, d);
    add(buf2, g, h);
    mulStrassenInternal(p6, buf1, buf2);

    // p7
    sub(buf1, a, c);
    add(buf2, e, f);
    mulStrassenInternal(p7, buf1, buf2);

    // collect everything
    // q1
    sub(buf1, p4, p2);
    add(buf2, p5, p6);
    add(topleft, buf1, buf2);
    // q2
    add(topright, p1, p2);
    // q3
    add(botleft, p3, p4);
    // q4
    sub(buf1, p1, p3);
    sub(buf2, p5, p7);
    add(botright, buf1, buf2);

    free(p1.mat);
    free(p2.mat);
    free(p3.mat);
    free(p4.mat);
    free(p5.mat);
    free(p6.mat);
    free(p7.mat);
    free(buf1.mat);
    free(buf2.mat);
}

void mulStrassen(size_t n, MatT* buf, MatT* lhs, MatT* rhs) {
    assert(__builtin_popcount(n) == 1);

    mulStrassenInternal(SUBM(buf, n, n, 0, 0), SUBM(lhs, n, n, 0, 0), SUBM(rhs, n, n, 0, 0));
}
