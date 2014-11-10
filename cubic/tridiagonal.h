// efficiently solve tridiagonal matrices in linear time and space
// See https://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm

#pragma once

#define T double

struct row {
    // a gets reused to pass back the results
    T a, b, c, y;
};

// mutates the original matrix
void solve_tridiagonal(struct row *mat, int n);
