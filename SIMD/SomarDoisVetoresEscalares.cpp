#include <iostream>
#include <cstdlib>
                                                        
int main(int argc, char** argv) {
    int length = std::atoi(argv[1]);

    if (length <= 0) {
        std::cerr << "Tamanho inválido\n";
        return 1;
    }

    float* a = new float[length];
    float* b = new float[length];
    float* c = new float[length];

    // Inicialização
    for (int i = 0; i < length; i++) {
        a[i] = i + 1.0f;
        b[i] = (i + 1.0f) * 10.0f;
    }

    // Soma escalar
    for (int i = 0; i < length; i++) {
        c[i] = a[i] + b[i];
    }

    std::cout << "Resultado:\n";
    for (int i = 0; i < length; i++) {
        std::cout << c[i] << " ";
    }
    std::cout << "\n";

    delete[] a;
    delete[] b;
    delete[] c;

    return 0;
}
