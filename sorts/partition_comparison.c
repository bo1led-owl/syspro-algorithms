#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void swap(int* a, size_t i, size_t j) {
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
}

typedef struct {
    size_t l, h;
} PartitionResult;

static PartitionResult lomutoPartition(int* a, size_t low, size_t high) {
    size_t pivot_index = low + rand() % (high - low + 1);
    int pivot = a[pivot_index];

    size_t l, h;
    l = h = low;

    for (size_t c = low; c <= high; ++c) {
        if (a[c] < pivot) {
            int tmp = a[c];
            a[c] = a[h];
            a[h] = a[l];
            a[l] = tmp;
            ++l;
            ++h;
        } else if (a[c] == pivot) {
            swap(a, h, c);
            ++h;
        }
    }

    return (PartitionResult){l, h};
}

static size_t lomutoPartitionBranchFree(int* a, size_t first, size_t last) {
    if (last - first < 2) return first;
    --last;
    if (a[first] > a[last]) swap(a, first, last);
    size_t pivot_pos = first;
    int pivot = a[first];
    do {
        ++first;
    } while (a[first] < pivot);
    for (size_t read = first + 1; read < last; ++read) {
        int x = a[read];
        int smaller = -(int)(x < pivot);
        int delta = smaller & (read - first);
        a[first + delta] = a[first];
        a[read - delta] = x;
        first -= smaller;
    }
    --first;
    a[pivot_pos] = a[first];
    a[first] = pivot;
    return first;
}

static size_t hoarePartition(int* a, size_t l, size_t r) {
    size_t pivot_index = l + rand() % (r - l + 1);
    int pivot = a[pivot_index];

    size_t i = l - 1;
    size_t j = r + 1;

    for (;;) {
        do {
            ++i;
        } while (a[i] < pivot);

        do {
            --j;
        } while (a[j] > pivot);

        if (i >= j) {
            return j;
        }

        swap(a, i, j);
    }
}

static void quickSortHoare(int* a, size_t l, size_t r) {
    if (l >= r) {
        return;
    }

    size_t pivot = hoarePartition(a, l, r);
    quickSortHoare(a, l, pivot);
    quickSortHoare(a, pivot + 1, r);
}

static void quickSortLomuto(int* a, size_t l, size_t r) {
    if (l >= r) {
        return;
    }

    PartitionResult pivots = lomutoPartition(a, l, r);
    if (pivots.l > 0) quickSortLomuto(a, l, pivots.l - 1);
    quickSortLomuto(a, pivots.h, r);
}

static void quickSortLomutoBranchFree(int* a, size_t l, size_t r) {
    if (l >= r) {
        return;
    }

    size_t pivot = lomutoPartitionBranchFree(a, l, r + 1);
    quickSortLomutoBranchFree(a, l, pivot);
    quickSortLomutoBranchFree(a, pivot + 1, r);
}

#define NOOPT(x) __asm__("" ::"m"(x))

static const size_t sizes[] = {
    17,   167,  353,  563,   761,   991,   1217,  1453,  1669,  1933,  2179,   2417,   2689,   2927,
    3217, 3469, 5881, 6203,  6469,  6763,  7019,  7333,  7607,  7907,  8221,   8527,   8783,   9067,
    9371, 9643, 9923, 10007, 11239, 18583, 19937, 21187, 22441, 23719, 101101, 527183, 6291456};

#define N_TESTS (sizeof(sizes) / sizeof(size_t))

#define REPETITIONS 25

static struct timespec getTime(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ts;
}

static double getTimeDiff(struct timespec begin, struct timespec end) {
    return (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1e9;
}

typedef int* Ctx;

static void fill(int* a, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        a[i] = rand() % 65536;
    }
}

static Ctx initCtx(size_t n) {
    Ctx res = malloc(n * sizeof(int));

    return res;
}

static void freeCtx(Ctx c) {
    free(c);
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

typedef void (*SortFn)(int*, size_t, size_t);

static void measure(FILE* f, const char* name, SortFn sort, size_t n) {
    printf("Running %s for N=%zu...\n", name, n);
    Ctx c = initCtx(n);

    double results[REPETITIONS];
    for (size_t rep = 0; rep < REPETITIONS; ++rep) {
        fill(c, n);

        struct timespec start = getTime();
        sort(c, 0, n - 1);
        NOOPT(*c);
        struct timespec end = getTime();

        results[rep] = getTimeDiff(start, end);
    }

    double sample_average = getSampleAverage(results, REPETITIONS);
    double standard_deviation = getStandardDeviation(results, REPETITIONS);
    double geometric_mean = getGeometricMean(results, REPETITIONS);

    fprintf(f, "%lf %lf %lf|", sample_average, standard_deviation, geometric_mean);

    freeCtx(c);
}

int main(void) {
    srand(time(NULL));

    FILE* outfile = fopen("result.dat", "w");
    if (!outfile) {
        perror("Could not open file to write results");
        return 1;
    }

    for (size_t i = 0; i < N_TESTS; ++i) {
        size_t n = sizes[i];
        fprintf(outfile, "%zu|", n);
        measure(outfile, "Hoare", quickSortHoare, n);
        measure(outfile, "Lomuto naive", quickSortLomuto, n);
        measure(outfile, "Lomuto branch-free", quickSortLomutoBranchFree, n);
        fputc('\n', outfile);
    }

    fclose(outfile);
    return 0;
}
