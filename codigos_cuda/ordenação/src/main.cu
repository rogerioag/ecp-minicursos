// ============================================================
//                  Observações gerais
// ============================================================
/*
    Este é o arquivo principal do projeto, responsável por orquestrar a execução dos algoritmos de ordenação
    (Merge Sort e Radix Sort) em diferentes versões: sequencial, com threads e com CUDA (GPU).
    O fluxo geral é:
        1. Define os arquivos de entrada e seus tamanhos
        2. Gera os arquivos binários com dados aleatórios
        3. Executa cada versão dos algoritmos de ordenação
        4. Mede e registra os tempos de execução em um arquivo CSV
        5. Verifica se os arquivos foram ordenados corretamente após cada execução
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "utils/utils.h"
#include "ordenadores/sequencial/merge_sort_seq.h"
#include "ordenadores/threads/merge_sort_threads.h"
#include "ordenadores/cuda/merge_sort_cuda.cu"
#include "ordenadores/sequencial/radix_sort_seq.h"
#include "ordenadores/threads/radix_sort_threads.h"
#include "ordenadores/cuda/radix_sort_cuda.cu"


// ============================================================
//                  Função principal
// ============================================================
int main()
{
    // Define o número de threads para as versões paralelas
    const int num_threads = 8;

    // Define o número de arquivos de entrada e seus tamanhos
    const int num_entradas = 11;

    const long tamanho_arquivos[num_entradas] = {
        250000, 500000, 750000, 1000000,
        2500000, 5000000, 7500000, 10000000,
        25000000, 50000000, 100000000
    };

    // Define os caminhos dos arquivos binários de entrada
    const char *entradas[num_entradas] =
    {
        "dados/250k.bin", "dados/500k.bin", "dados/750k.bin", "dados/1m.bin",
        "dados/2m500.bin", "dados/5m.bin", "dados/7m500.bin", "dados/10m.bin",
        "dados/25m.bin", "dados/50m.bin", "dados/100m.bin"

    };

    // Cria e inicializa o arquivo CSV para registrar os tempos de execução
    FILE *csv = fopen("results/tempos.csv", "w");
    if (!csv) {
        perror("Erro ao abrir arquivo CSV para escrita");
        return 1;
    }
    fprintf(csv, "Algoritmo,Tamanho,Tempo\n");
    fclose(csv);

    /*
        Para cada algoritmo: 
            - gera os arquivos binarios com dados aleatórios
            - executa a ordenação
            - verifica se os arquivos estão ordenados corretamente
    */

    // Merge Sort Sequencial
    GerarArquivos(tamanho_arquivos, entradas, num_entradas);
    ExecMergeSeq(entradas, num_entradas, "results/tempos.csv");
    VerificarOrdenado(entradas, num_entradas);

    // Merge Sort com Threads (Não finalizado)
    GerarArquivos(tamanho_arquivos, entradas, num_entradas);
    ExecMergeThread(entradas, num_entradas, num_threads, "results/tempos.csv");
    VerificarOrdenado(entradas,num_entradas);

    // Merge Sort com CUDA (GPU)
    GerarArquivos(tamanho_arquivos, entradas, num_entradas);
    ExecMergeCuda(entradas, num_entradas, "results/tempos.csv");
    VerificarOrdenado(entradas,num_entradas);
    
    // Radix Sort Sequencial
    GerarArquivos(tamanho_arquivos, entradas, num_entradas);
    ExecRadixSeq(entradas, num_entradas, "results/tempos.csv");
    VerificarOrdenado(entradas, num_entradas);

    // Radix Sort com Threads **(WIP)
    // GerarArquivosUtils(tamanho_arquivos, entradas, num_entradas);
    // ExecRadixThread(entradas, num_entradas, num_threads, "results/tempos.csv");
    // VerificarOrdenado(entradas,num_entradas);   

    // Radix Sort com CUDA (GPU) **(WIP)
    // GerarArquivosUtils(tamanho_arquivos, entradas, num_entradas);
    // ExecRadixCuda(entradas, num_entradas, "results/tempos.csv");
    // VerificarOrdenado(entradas,num_entradas);

}
