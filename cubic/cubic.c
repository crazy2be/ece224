#include "cubic.h"
#include <assert.h>

#define X_DELTA 1

#define CLAMP_B 1
#define CLAMP_C 0

/* This algorithm is not entirely obvious.
 * It is a combination of the cubic spline formulas from the CS370 course
 * notes, and Thomas' algorithm for solving tridiagonal matrices.
 * See https://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm */

static inline T calculate_c_prime(T a, T b, T c, T c_prime_prev) {
    return c / (b - a * c_prime_prev);
}

static inline T calculate_y_prime(T y, T a, T b, T y_prime_prev, T c_prime_prev) {
    return (y - a * y_prime_prev) / (b - a * c_prime_prev);
}

void cubic_spline_solve(T *ys, T *ss, struct row *mat, int n, T left_clamp) {
    // the caller preallocates the intermediate matrix for efficiency
    // we then get to reuse the same memory many times

    if (n <= 0) {
        return;
    }

    /* Do the tridiagonal matrix solving.
     * Because the xs are assumed to be [0, 1, 2, ..., n-1], we can cheat a bit.
     * Some of the math becomes simpler, because X_DELTA is a constant 1.
     * Also, we can calculate most of the values on the fly, so we save a bit of
     * space. */

    // left boundary (clamped)
    mat[0].c = CLAMP_C / CLAMP_B;
    mat[0].y = left_clamp / CLAMP_B;

    for (int i = 1; i < n - 1; i++) {
        const T a_i = X_DELTA;
        const T b_i = 2 * (X_DELTA + X_DELTA);
        const T c_i = X_DELTA;

        // expression is simplified somewhat:
        // 3 * (X_DELTA * (ys[i] - ys[i-1]) / X_DELTA + X_DELTA * (ys[i+1] - ys[i]) / X_DELTA
        // = 3 * ((ys[i] - ys[i-1]) + (ys[i+1] - ys[i])
        // = 3 * (ys[i + 1] - ys[i-1])
        const T y_i = 3 * (ys[i + 1] - ys[i - 1]);

        mat[i].c = calculate_c_prime(a_i, b_i, c_i, mat[i - 1].c);
        mat[i].y = calculate_y_prime(y_i, a_i, b_i, mat[i - 1].y, mat[i - 1].c);
    }

    // right boundary (free)

    {
        const T a_n = 0.5;
        const T b_n = 1;
        const T y_n = 1.5 * (ys[n - 1] - ys[n - 2]) / X_DELTA;
        ss[n - 1] = calculate_y_prime(y_n, a_n, b_n, mat[n - 2].y, mat[n - 2].c);
    }

    for (int i = n - 2; i >= 0; i--) {
        ss[i] = mat[i].y - mat[i].c * ss[i + 1];
    }
}

T cubic_spline_interpolate(T *ys, T *ss, int n, T x) {
    assert(x >= 0 && x <= n - 1);
    int i = (int) x;

    const T y_prime = ys[i + 1] - ys[i];
    T x_prime = x - (double) i;

    const T a = ys[i];
    const T b = ss[i];
    const T c = (3 * y_prime - 2 * ss[i] - ss[i + 1]) / X_DELTA;
    const T d = (ss[i + 1] + ss[i] - 2 * y_prime) / (X_DELTA * X_DELTA);

    T ret;
    ret = a;

    ret += b * x_prime;

    x_prime *= x_prime;
    ret += c * x_prime;

    x_prime *= x_prime;
    ret += d * x_prime;

    return ret;
}
