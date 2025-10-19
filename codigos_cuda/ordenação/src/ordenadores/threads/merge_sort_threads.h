// ============================================================
//                  Observações gerais
// ============================================================
/*
    Este arquivo implementa o algoritmo Merge Sort Bottom-Up de forma paralela na CPU usando threads POSIX.
    O fluxo geral é:
        1. Ler vetores de arquivos binários (int)
        2. Ordenar os dados usando Merge Sort (bottom-up) com threads
        3. Medir o tempo de execução
        4. Regravar o arquivo com os dados ordenados
        5. Registrar tempos em CSV

    O Merge Sort é um algoritmo de ordenação baseado na técnica "dividir para conquistar".
    Esta versão distribui os merges entre múltiplas threads para acelerar o processamento.
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <pthread.h>

using namespace std;

// ============================================================
//                  Estrutura de dados para threads
// ============================================================
/*
    Estrutura ThreadDados: usada para passar parâmetros para cada thread.

    Campos:
        - vetor: ponteiro para o array de inteiros a ser ordenado
        - inicio_merge: índice do primeiro merge que esta thread deve processar
        - fim_merge: índice do último merge que esta thread deve processar
        - tamanho_subvetor: tamanho atual das sublistas a serem mescladas
        - tamanho_total: tamanho total do vetor
 */
struct ThreadDados {
    int *vetor;
    int inicio_merge;
    int fim_merge;
    int tamanho_subvetor;
    int tamanho_total;              
};

// ============================================================
//                  FUNÇÃO DE MESCLAGEM (MERGE)
// ============================================================
/*
    MergeThread: mescla dois subvetores adjacentes já ordenados em um único segmento ordenado.

    Parâmetros:
        - vetor: ponteiro para o array de inteiros a ser ordenado
        - começo: índice inicial do primeiro subvetor
        - meio: índice final do primeiro subvetor
        - fim: índice final do segundo subvetor

    Funcionamento:
        - Cria vetores auxiliares para os segmentos esquerdo e direito
        - Copia os elementos dos subvetores para os auxiliares
        - Mescla os dois subvetores de volta ao vetor principal, mantendo a ordem
 */
void MergeThread(int *vetor, int começo, int meio, int fim){
    int tam_esquerda = meio - começo + 1;
    int tam_direita = fim - meio;

    int *vet_esq = new int[tam_esquerda];
    int *vet_dir = new int[tam_direita];

    // Copia elementos para os vetores auxiliares
    int idx_esq = 0;
    while(idx_esq < tam_esquerda){
        vet_esq[idx_esq] = vetor[começo + idx_esq];
        idx_esq++;
    }
    
    int idx_dir = 0;
    while(idx_dir < tam_direita){
        vet_dir[idx_dir] = vetor[meio + 1 + idx_dir];
        idx_dir++;
    }

    // Mescla os vetores auxiliares de volta ao vetor principal
    idx_esq = 0;
    idx_dir = 0;
    int idx = começo;
    while(idx_esq < tam_esquerda && idx_dir < tam_direita){
        if(vet_esq[idx_esq] <= vet_dir[idx_dir]){
            vetor[idx] = vet_esq[idx_esq];
            idx_esq++;
        } else {
            vetor[idx] = vet_dir[idx_dir];
            idx_dir++;
        }
        idx++;
    }

    // Copia o restante dos elementos, se houver
    while(idx_esq < tam_esquerda){
        vetor[idx] = vet_esq[idx_esq];
        idx_esq++;
        idx++;
    }

    while(idx_dir < tam_direita){
        vetor[idx] = vet_dir[idx_dir];
        idx_dir++;
        idx++;
    }

    delete[] vet_esq;
    delete[] vet_dir;
}

// ============================================================
//                  FUNÇÃO DE TRABALHO DAS THREADS
// ============================================================
/*
    ThreadMergeWorker: função executada por cada thread para processar uma faixa de merges.

    Parâmetros:
        - arg: ponteiro para estrutura ThreadDados contendo os parâmetros da thread

    Funcionamento:
        * Para cada merge atribuído à thread:
        - Calcula os indices dos subvetores a serem mesclados
        - Chama MergeThread para realizar a mesclagem
 */
void* ThreadMergeWorker(void *arg)
{
    ThreadDados *dados = (ThreadDados *)arg;
    
    int idx_merge = dados->inicio_merge;
    while(idx_merge < dados->fim_merge)
    {
        int começo = idx_merge * dados->tamanho_subvetor * 2;
        
        if(começo >= dados->tamanho_total - 1)
        {
            break;
        }
            
        int meio = min(começo + dados->tamanho_subvetor - 1, dados->tamanho_total - 1);
        int fim = min(começo + 2 * dados->tamanho_subvetor - 1, dados->tamanho_total - 1);
        
        if(meio < fim)
        {
            MergeThread(dados->vetor, começo, meio, fim);
        }
        
        idx_merge++;
    }
    
    delete dados;
    pthread_exit(0);
}

// ============================================================
//                  FUNÇÃO PRINCIPAL MERGE SORT COM THREADS
// ============================================================
/*
    MergeSortThread: ordena um vetor de inteiros usando o algoritmo Merge Sort iterativo (bottom-up) com threads.

    Parâmetros:
        - vetor: ponteiro para o array de inteiros a ser ordenado
        - tamanho_total: número de elementos no array
        - num_threads: número de threads a serem utilizadas

    Funcionamento:
        * Para cada tamanho de subvetor (1, 2, 4, ...):
        - Divide os merges entre as threads disponíveis
        - Cada thread processa uma faixa de merges
        - Aguarda todas as threads terminarem antes de dobrar o tamanho dos subvetores
 */
void MergeSortThread(int *vetor, int tamanho_total, int num_threads)
{
    if(tamanho_total <= 1)
    {
        return;
    }
    
    int tamanho_subvetor = 1;
    while(tamanho_subvetor < tamanho_total)
    {
        int total_merges = (tamanho_total + (tamanho_subvetor * 2 - 1)) / (tamanho_subvetor * 2);
        int threads_usadas = min(num_threads, total_merges);
        
        pthread_t *threads = new pthread_t[threads_usadas];
        
        for(int t = 0; t < threads_usadas; t++)
        {
            ThreadDados *dados = new ThreadDados;
            dados->vetor = vetor;
            dados->tamanho_subvetor = tamanho_subvetor;
            dados->tamanho_total = tamanho_total;
            dados->inicio_merge = (total_merges * t) / threads_usadas;
            dados->fim_merge = (total_merges * (t + 1)) / threads_usadas;
            
            pthread_create(&threads[t], NULL, ThreadMergeWorker, dados);
        }
        
        for(int t = 0; t < threads_usadas; t++)
        {
            pthread_join(threads[t], NULL);
        }
        
        delete[] threads;
        tamanho_subvetor *= 2;
    }
}
// ============================================================
//             FUNÇÃO DE EXECUÇÃO E MEDIÇÃO DE TEMPO
// ============================================================
/*
    ExecMergeThread: executa o Merge Sort com threads para múltiplos arquivos binários
    contendo inteiros, mede o tempo de ordenação e registra os resultados em CSV.

    Parâmetros:
        - entradas: array de caminhos (const char*) para arquivos binários
        - num_entradas: número de entradas no array
        - num_threads: número de threads a serem utilizadas
        - csv_saida: caminho do arquivo CSV de saída onde serão registrados os tempos

    Funcionamento:
        * Para cada arquivo:
        - Abre o arquivo e determina o número de inteiros
        - Lê os dados para um vetor alocado dinamicamente
        - Mede o tempo de ordenação usando chrono
        - Ordena os dados com MergeSortThread
        - Registra o tempo no arquivo CSV
        - Regrava o arquivo com os dados ordenados
        - Libera memória utilizada
 */
void ExecMergeThread(const char **entradas, int num_entradas, int num_threads, const char *csv_saida)
{
    FILE *csv = fopen(csv_saida, "a");
    FILE *thread_merge_csv = fopen("results/threads/merge_thread.csv", "a");
    if (!csv || !thread_merge_csv)
    {
        perror("Erro ao abrir arquivos de saída CSV");
        return;
    }

    fprintf(thread_merge_csv, "Tamanho,Tempo\n");

    for(int i = 0; i < num_entradas; i++)
    {
        FILE *file = fopen(entradas[i], "rb+");
        if(!file)
        {
            perror(entradas[i]);
            return;
        }

        fseek(file, 0, SEEK_END);
        long tamanho = ftell(file) / sizeof(int);
        fseek(file, 0, SEEK_SET);

        int *v = new int[tamanho];
        if(fread(v, sizeof(int), tamanho, file) != (size_t)tamanho)
        {
            perror("Erro ao ler o arquivo");
            fclose(file);
            delete[] v;
            continue;
        }

        auto start = chrono::high_resolution_clock::now();
        MergeSortThread(v, tamanho, num_threads);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        double tempo = elapsed.count();

        printf("MergeSort Threads - Tempo para ordenar %s: %f s\n", entradas[i], tempo);

        fprintf(csv, "MergeSort - Threads,%ld,%f\n", tamanho, tempo);
        fprintf(thread_merge_csv, "%ld,%f\n", tamanho, tempo);

        fseek(file, 0, SEEK_SET);
        if(fwrite(v, sizeof(int), tamanho, file) != (size_t)tamanho)
        {
            perror("Erro ao escrever no arquivo");
            fclose(file);
            delete[] v;
            continue;
        }

        fclose(file);
        delete[] v;
    }

    fclose(csv);
    fclose(thread_merge_csv);
}