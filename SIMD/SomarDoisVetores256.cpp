#include <immintrin.h>
#include <iostream>


int main(int argc, char const *argv[])
{
    alignas(32) float a[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    alignas(32) float b[8] = {10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0};
    alignas(32) float c[8];
    
    __m256 vec_a = _mm256_load_ps(a);
    __m256 vec_b = _mm256_load_ps(b);
    __m256 vec_c = _mm256_add_ps(vec_a, vec_b);
    _mm256_store_ps(c, vec_c);

    std::cout << "Resultado da soma dos vetores:\n";
    for (int i = 0; i < 8; ++i) {
        std::cout << c[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
