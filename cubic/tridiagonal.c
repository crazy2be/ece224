#include "tridiagonal.h"

void solve_tridiagonal(struct row *mat, int n) {
    if (n <= 0) {
        return;
    }

    mat[0].c = mat[0].c / mat[0].b;
    mat[0].y = mat[0].y / mat[0].b;

    for (int i = 1; i < n; i++) {
        mat[i].c = mat[i].c / (mat[i].b - mat[i].a * mat[i-1].c);
        mat[i].y = (mat[i].y - mat[i].a * mat[i - 1].y)
                   / (mat[i].b - mat[i].a * mat[i - 1].c);
    }

    mat[n - 1].a = mat[n - 1].y;
    for (int i = n - 2; i >= 0; i--) {
        mat[i].a = mat[i].y - mat[i].c * mat[i + 1].a;
    }
}
