#include <immintrin.h>
#include <iostream>


int main(int argc, char const *argv[])
{
    alignas(64) float a[16] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};
    alignas(64) float b[16] = {10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 110.0, 120.0, 130.0, 140.0, 150.0, 160.0};
    alignas(64) float c[16];
    
    __m512 vec_a = _mm512_load_ps(a);
    __m512 vec_b = _mm512_load_ps(b);
    __m512 vec_c = _mm512_add_ps(vec_a, vec_b);
    _mm512_store_ps(c, vec_c);

    std::cout << "Resultado da soma dos vetores:\n";
    for (int i = 0; i < 16; ++i) {
        std::cout << c[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
