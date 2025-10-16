// ============================================================
//                  Observações gerais
// ============================================================
/*
    Este arquivo implementa o algoritmo Merge Sort Botton Up de forma sequencial na CPU.
    O fluxo geral é:
        1. Ler vetores de arquivos binários (int)
        2. Ordenar os dados usando Merge Sort (bottom-up)
        3. Medir o tempo de execução
        4. Regravar o arquivo com os dados ordenados
        5. Registrar tempos em CSV

    O Merge Sort é um algoritmo de ordenação baseado na técnica "dividir para conquistar".
    Ele divide o vetor em subvetores, ordena cada subvetor e depois mescla (merge) os resultados.
    Esta implementação utiliza a versão iterativa (bottom-up), que evita chamadas recursivas.
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>

using namespace std;



// ============================================================
//                  FUNÇÃO DE MESCLAGEM (MERGE)
// ============================================================
/*
    MergeSeq: mescla dois subvetores adjacentes já ordenados em um único segmento ordenado.

    Parâmetros:
        - vetor: ponteiro para o array de inteiros a ser ordenado
        - começo: índice inicial do primeiro subvetor
        - meio: índice final do primeiro subvetor
        - fim: índice final do segundo subvetor

    Funcionamento:
        - Cria vetores auxiliares para os segmentos esquerdo e direito
        - Copia os elementos dos subvetores para os auxiliares
        - Mescla os dois subvetores de volta ao vetor principal, mantendo a ordem
        - Garante estabilidade na ordenação
 */
void MergeSeq(int *vetor, int começo, int meio, int fim)
{
    int tam_esquerda = meio - começo + 1;
    int tam_direita = fim - meio;

    int *vet_esq = new int[tam_esquerda];
    int *vet_dir = new int[tam_direita];

    // Copia elementos para os vetores auxiliares
    int idx_esq = 0;
    while(idx_esq < tam_esquerda)
    {
        vet_esq[idx_esq] = vetor[começo + idx_esq];
        idx_esq++;
    }
    
    int idx_dir = 0;
    while(idx_dir < tam_direita)
    {
        vet_dir[idx_dir] = vetor[meio + 1 + idx_dir];
        idx_dir++;
    }

    // Mescla os vetores auxiliares de volta ao vetor principal
    idx_esq = 0;
    idx_dir = 0;
    int idx = começo;
    while(idx_esq < tam_esquerda && idx_dir < tam_direita)
    {
        if(vet_esq[idx_esq] <= vet_dir[idx_dir])
        {
            vetor[idx] = vet_esq[idx_esq];
            idx_esq++;
        } else {
            vetor[idx] = vet_dir[idx_dir];
            idx_dir++;
        }
        idx++;
    }

    // Copia o restante dos elementos, se houver
    while(idx_esq < tam_esquerda)
    {
        vetor[idx] = vet_esq[idx_esq];
        idx_esq++;
        idx++;
    }

    while(idx_dir < tam_direita)
    {
        vetor[idx] = vet_dir[idx_dir];
        idx_dir++;
        idx++;
    }

    delete[] vet_esq;
    delete[] vet_dir;
}

// ============================================================
//                  FUNÇÃO PRINCIPAL MERGE SORT
// ============================================================
/*
    MergeSortSeq: ordena um vetor de inteiros usando o algoritmo Merge Sort iterativo (bottom-up).

    Parâmetros:
        - vetor: ponteiro para o array de inteiros a ser ordenado
        - n: número de elementos no array

    Funcionamento:
        - Começa com subvetores de tamanho 1 e vai dobrando o tamanho a cada iteração
        - Para cada par de subvetores adjacentes, chama MergeSeq para mesclar
        - Repete até que todo o vetor esteja ordenado
 */
void MergeSortSeq(int *vetor, int n)
{
    int tamanho = 1;
    while(tamanho < n)
    {
        int inicio = 0;
        while(inicio < n - 1)
        {
            int começo = inicio;
            int meio = min(inicio + tamanho - 1, n - 1);
            int fim = min(inicio + 2 * tamanho - 1, n - 1);
            
            if(meio < fim)
                MergeSeq(vetor, começo, meio, fim);
            
            inicio += 2 * tamanho;
        }
        
        tamanho *= 2;
    }
}

// ============================================================
//             FUNÇÃO DE EXECUÇÃO E MEDIÇÃO DE TEMPO
// ============================================================
/*
    ExecMergeSeq: executa o Merge Sort sequencial para múltiplos arquivos binários
    contendo inteiros, mede o tempo de ordenação e registra os resultados em CSV.

    Parâmetros:
        - entradas: array de caminhos (const char*) para arquivos binários
        - num_entradas: número de entradas no array
        - csv_saida: caminho do arquivo CSV de saída onde serão registrados os tempos

    Funcionamento:
        * Para cada arquivo:
        - Abre o arquivo e determina o número de inteiros
        - Lê os dados para um vetor alocado dinamicamente
        - Mede o tempo de ordenação usando chrono
        - Ordena os dados com MergeSortSeq
        - Registra o tempo no arquivo CSV
        - Regrava o arquivo com os dados ordenados
        - Libera memória utilizada
 */
void ExecMergeSeq(const char **entradas, int num_entradas, const char *csv_saida)
{
    FILE *csv = fopen(csv_saida, "a");
    if (!csv)
    {
        perror("Erro ao abrir arquivo CSV");
        return;
    }

    for (int i = 0; i < num_entradas; i++)
    {
        FILE *file = fopen(entradas[i], "rb+"); // abre para leitura/escrita binária

        if(!file)
        {
            perror(entradas[i]);
            return;
        }

        fseek(file, 0, SEEK_END);
        long tamanho = ftell(file) / sizeof(int);
        fseek(file, 0, SEEK_SET);

        int *vetor = new int[tamanho];

        if(fread(vetor, sizeof(int), tamanho, file) != (size_t)tamanho)
        {
            perror("Erro ao ler o arquivo");
            fclose(file);
            delete[] vetor;
            continue;
        }

        auto start = chrono::high_resolution_clock::now();
        MergeSortSeq(vetor, tamanho);
        auto end = chrono::high_resolution_clock::now();

        chrono::duration<double> elapsed = end - start;
        double tempo = elapsed.count();

        printf("Merge Sort Sequencial - Tempo para ordenar %s: %f s\n", entradas[i], tempo);
        fprintf(csv, "MergeSort - Sequencial,%ld,%f\n", tamanho, tempo);

        fseek(file, 0, SEEK_SET);
        if(fwrite(vetor, sizeof(int), tamanho, file) != (size_t)tamanho)
        {
            perror("Erro ao escrever no arquivo");
            fclose(file);
            delete[] vetor;
            continue;
        }    

        fclose(file);
        delete[] vetor;
    }

    fclose(csv);
}