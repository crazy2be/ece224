#pragma once

#include "tridiagonal.h"

// xs is assumed to be [0, 1, 2, ...]
// this makes the math substantially more simple
// results are returned in the `a` member of mat_temp
void cubic_spline_solve(T *ys, struct row *mat_temp, int n, T left_clamp);

T cubic_spline_interpolate(T *ys, struct row *mat, int n, T x);
