#include "cubic.h"
#include <stdio.h>
#include <float.h>
#include <assert.h>

int main(void) {
    const int n = 4;
    double ys[] = { 2, 3, 5, 1 };
    struct row rows[n];
    cubic_spline_solve(ys, rows, n, 0);
    /* for (int i = 0; i < n; i++) { */
    /*     printf("%f, %f\n", ys[i], rows[i].a); */
    /* } */

    for (int i = 0; i < n; i++) {
        double x = (double) i;
        double y = cubic_spline_interpolate(ys, rows, n, x);
        assert(y - ys[i] < DBL_EPSILON);
    }

    for (double x = 0; x <= 3; x += 0.1) {
        double y = cubic_spline_interpolate(ys, rows, n, x);
        printf("%f, %f\n", x, y);
    }

    return 0;
}

