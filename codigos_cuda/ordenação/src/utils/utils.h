#include <iostream>
#include <stdio.h>
#include <stdlib.h>

void imprimir_vetor(const char **entrada, int num_entradas)
{
    for(int i = 0; i < num_entradas; i++)
    {
        FILE *file = fopen(entrada[i], "rb");
        if(!file)
        {
            perror(entrada[i]);
            continue;
        }

        fseek(file, 0, SEEK_END);
        long tamanho = ftell(file) / sizeof(int);
        fseek(file, 0, SEEK_SET);

        int *v = new int[tamanho];
        if(fread(v, sizeof(int), tamanho, file) != tamanho)
        {
            perror("Erro ao ler o arquivo");
            fclose(file);
            delete[] v;
            continue;
        }
        fclose(file);


        printf("Vetor %s:\n", entrada[i]);
    
        for(long j = 0; j < tamanho; j++)
        {
            printf("%d ", v[j]);
        }

        printf("\n\n");

        delete[] v;
    }
}

void VerificarOrdenado(const char **arquivos, const int num_entradas)
{
    for (int i = 0; i < num_entradas; i++)
    {
        const char *path = arquivos[i];
        FILE *file = fopen(path, "rb");
        if (!file)
        {
            perror(path);
            continue;
        }

        int anterior, atual;
        size_t lidos;

        lidos = fread(&anterior, sizeof(int), 1, file);
        if (lidos != 1) {
            if (feof(file)) {
                printf("O arquivo %s está vazio.\n", path);
            } else {
                perror("Erro ao ler o arquivo");
            }
            fclose(file);
            continue;
        }

        long pos = 1; // posição do elemento atual (segunda leitura será pos=1)
        bool desordenado = false;
        while ((lidos = fread(&atual, sizeof(int), 1, file)) == 1) {
            if (atual < anterior) {
                printf("Erro: arquivo %s está desordenado na posição %ld (anterior=%d > atual=%d)\n",
                       path, pos, anterior, atual);
                desordenado = true;
                break;
            }
            anterior = atual;
            pos++;
        }

        if (!desordenado) {
            printf("O arquivo %s está ordenado.\n", path);
        }

        fclose(file);
    }
}

void GerarArquivos(const long *tamanho_arquivos, const char **nomes_arquivos, const int num_arquivos) 
{
    for (int i = 0; i < num_arquivos; i++) 
    {
        long n = tamanho_arquivos[i];
        const char *path = nomes_arquivos[i];

        FILE *file = fopen(path, "wb");
        if (!file) 
        {
            perror(path);
            continue;
        }

        for (long j = 0; j < n; j++) 
        {
            int num = rand() % 100000000;  // gera número aleatório entre 0 e 100 milhões
            if (fwrite(&num, sizeof(int), 1, file) != 1) 
            {
                perror("Erro ao escrever no arquivo");
                fclose(file);
                break;
            }
        }

        fclose(file);
        printf("Gerado: %s com %ld inteiros\n", path, n);
    }

    printf("\n");

}