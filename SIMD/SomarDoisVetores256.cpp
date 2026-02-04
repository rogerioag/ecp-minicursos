#include <immintrin.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv) {
    int length = std::atoi(argv[1]);
   

    if (length <= 0) {
        std::cerr << "Tamanho inválido\n";
        return 1;
    }

    // Alocação alinhada (32 bytes)
    float* a = (float*) aligned_alloc(32, length * sizeof(float));
    float* b = (float*) aligned_alloc(32, length * sizeof(float));
    float* c = (float*) aligned_alloc(32, length * sizeof(float));

    // Inicialização
    for (int i = 0; i < length; i++) {
        a[i] = i + 1.0f;
        b[i] = (i + 1.0f) * 10.0f;
    }

    int i = 0;

    // Soma vetorizada (8 floats por vez)
    for (; i + 7 < length; i += 8) {
        __m256 va = _mm256_load_ps(&a[i]);
        __m256 vb = _mm256_load_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_store_ps(&c[i], vc);
    }

    // Resto escalar
    for (; i < length; i++) {
        c[i] = a[i] + b[i];
    }

    std::cout << "Resultado:\n";
    for (int i = 0; i < length; i++) {
        std::cout << c[i] << " ";
    }
    std::cout << "\n";

    free(a);
    free(b);
    free(c);

    return 0;
}
