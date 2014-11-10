#include "cubic.h"
#include <assert.h>

void cubic_spline_solve(T *ys, struct row *mat_temp, int n, T left_clamp) {
    // the caller preallocates the intermediate matrix for efficiency
    // we get to reuse the same memory many times

    if (n <= 0) {
        return;
    }

    // set up first row (left boundary)
    mat_temp[0].b = 1;
    mat_temp[0].c = 0;
    mat_temp[0].y = left_clamp;

    // set up interior rows
    for (int i = 1; i < n - 1; i++) {
        mat_temp[i].y = ys[i];
        mat_temp[i].a = 1;
        mat_temp[i].b = 2;
        mat_temp[i].c = 1;
    }

    // set up last row (right boundary)
    mat_temp[n - 1].b = 1;
    mat_temp[n - 1].a = 0.5;
    mat_temp[n - 1].y = 1.5 * (ys[n - 1] - ys[n - 2]);

    solve_tridiagonal(mat_temp, n);
}

T cubic_spline_interpolate(T *ys, struct row *mat, int n, T x) {
    assert(x >= 0 && x <= n - 1);
    int i = (int) x;
    T y_prime, x_prime;

    y_prime = ys[i + 1] - ys[i];
    x_prime = x - i;

    T ret;
    ret = ys[i];

    ret += (mat[i].a) * x_prime;

    x_prime *= x_prime;
    ret += (3 * y_prime - 2 * mat[i].a - mat[i + 1].a) * x_prime;

    x_prime *= x_prime;
    ret += (mat[i + 1].a + mat[i].a - 2 * y_prime) * x_prime;

    return ret;
}
