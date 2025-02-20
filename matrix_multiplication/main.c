#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matrix.h"

#define NOOPT(x) __asm__("" ::"m"(x))

typedef struct {
    bool naive;
    bool bad;
    bool strassen;
    // sizes are powers of two in range [2^low, 2^high]
    size_t low;
    size_t high;
} Benchmark;

static const Benchmark bench = (Benchmark){
    .naive = true,
    .bad = true,
    .strassen = true,
    .low = 6,
    .high = 10,
};

#define REPETITIONS 25

static struct timespec getTime(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ts;
}

static double getTimeDiff(struct timespec begin, struct timespec end) {
    return (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1e9;
}

typedef struct {
    MatT* buf;
    MatT* lhs;
    MatT* rhs;
} Ctx;

static Ctx initCtx(size_t n) {
    Ctx res = (Ctx){
        .buf = malloc(n * n * sizeof(MatT)),
        .lhs = malloc(n * n * sizeof(MatT)),
        .rhs = malloc(n * n * sizeof(MatT)),
    };

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            res.lhs[i * n + j] = rand() % 256;
            res.rhs[i * n + j] = rand() % 256;
        }
    }

    return res;
}

static void freeCtx(Ctx c) {
    free(c.buf);
    free(c.lhs);
    free(c.rhs);
}

static double getSampleAverage(const double* data, size_t n) {
    double sum = 0.0;
    for (size_t i = 0; i < n; ++i) {
        sum += data[i];
    }
    return sum / n;
}

static double getStandardDeviation(const double* data, size_t n) {
    double sample_avg = getSampleAverage(data, n);

    double res = 0.0;
    for (size_t i = 0; i < n; ++i) {
        res += (data[i] - sample_avg) * (data[i] - sample_avg);
    }
    res /= n;
    return sqrt(res);
}

static double getGeometricMean(const double* data, size_t n) {
    double res = 1.0;
    for (size_t i = 0; i < n; ++i) {
        res *= data[i];
    }
    return pow(res, 1 / (double)n);
}

static void measure(FILE* f, const char* name, MulFn mul, size_t n) {
    printf("Running %s for N=%zu...\n", name, n);
    Ctx c = initCtx(n);

    double results[REPETITIONS];
    for (size_t rep = 0; rep < REPETITIONS; ++rep) {
        struct timespec start = getTime();
        mul(n, c.buf, c.lhs, c.rhs);
        NOOPT(c);
        struct timespec end = getTime();

        results[rep] = getTimeDiff(start, end);
    }

    double sample_average = getSampleAverage(results, REPETITIONS);
    double standard_deviation = getStandardDeviation(results, REPETITIONS);
    double geometric_mean = getGeometricMean(results, REPETITIONS);

    fprintf(f, "%lf %lf %lf|", sample_average, standard_deviation, geometric_mean);

    freeCtx(c);
}

int main() {
    srand(time(NULL));

    FILE* outfile = fopen("result.dat", "w");
    if (!outfile) {
        perror("Could not open file to write results");
        return 1;
    }

    if (bench.naive) {
        fprintf(outfile, "naive ");
    }
    if (bench.bad) {
        fprintf(outfile, "bad ");
    }
    if (bench.strassen) {
        fprintf(outfile, "strassen");
    }
    fputc('\n', outfile);

    for (size_t i = bench.low; i <= bench.high; ++i) {
        size_t n = (size_t)1 << i;
        fprintf(outfile, "%zu|", n);
        if (bench.naive) {
            measure(outfile, "naive", mulNaive, n);
        }
        if (bench.bad) {
            measure(outfile, "bad", mulBad, n);
        }
        if (bench.strassen) {
            measure(outfile, "strassen", mulStrassen, n);
        }
        fputc('\n', outfile);
    }

    fclose(outfile);
    return 0;
}
