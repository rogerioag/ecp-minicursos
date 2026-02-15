#include <immintrin.h>
#include <iostream>
#include <cstdlib>


__m256d a, b, c;

int main()
{
    for (int i = 0; i < 512; i++)
    {
        c[i] = a[i] * b[i];
    }
    c = _mm256_mul_pd(a, b);
    return 0;
}
