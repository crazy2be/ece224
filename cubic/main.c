#include "cubic.h"
#include <stdio.h>
#include <float.h>
#include <assert.h>

int main(void) {
    const int n = 4;
    double ys[] = { 2, 4, 1, 6 };
    double ss[4];
    struct row rows[n];
    cubic_spline_solve(ys, ss, rows, n, 0);

    for (int i = 0; i < n; i++) {
        double x = (double) i;
        double y = cubic_spline_interpolate(ys, ss, n, x);
        assert(y - ys[i] < DBL_EPSILON);
    }

    for (double x = 0; x <= 3; x += 0.01) {
        double y = cubic_spline_interpolate(ys, ss, n, x);
        printf("%f, %f\n", x, y);
    }

    return 0;
}

