// ============================================================
//                  Observações gerais
// ============================================================
/*
    Este arquivo implementa o algoritmo Radix Sort de forma sequencial na CPU.
    O fluxo geral é:
        1. Ler vetores de arquivos binários (int)
        2. Ordenar os dados usando Radix Sort (base decimal)
        3. Medir o tempo de execução
        4. Regravar o arquivo com os dados ordenados
        5. Registrar tempos em CSV

    O Radix Sort é um algoritmo de ordenação não-comparativo que ordena inteiros
    processando cada dígito individualmente, da menor para a maior posição (unidades,
    dezenas, centenas, etc). Para cada posição, utiliza o Counting Sort como sub-rotina
    estável para garantir a ordenação correta.
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <iostream>

using namespace std;

// ============================================================
//                  FUNÇÃO COUNTING SORT (AUXILIAR)
// ============================================================
/*
    CountingSort: ordena o vetor de inteiros considerando apenas o dígito
    correspondente à posição indicada por 'expoente' (1=unidades, 10=dezenas, ...).

    Parâmetros:
        - vetor:    ponteiro para o array de inteiros a ser ordenado
        - tamanho:  número de elementos no array
        - expoente: potência de 10 que indica qual dígito será usado na ordenação
 
    Funcionamento:
        - Conta quantas vezes cada dígito (0-9) aparece na posição 'expoente'
        - Calcula a posição final de cada elemento usando contagem cumulativa
        - Reorganiza o vetor de acordo com o dígito atual
        - Copia o resultado ordenado de volta para o vetor original
 */
void CountingSort(int *vetor, int tamanho, int expoente) 
{
    int *saida = new int[tamanho]; // vetor auxiliar para saída ordenada
    int count[10] = {0};           // contadores para cada dígito (0-9)

    // Conta a ocorrência de cada dígito na posição 'expoente'
    for (int i = 0; i < tamanho; i++)
    {
        count[(vetor[i] / expoente) % 10]++;
    }

    // Atualiza count[i] para que ele contenha a posição real do dígito no vetor de saída
    for (int i = 1; i < 10; i++)
    {
        count[i] += count[i - 1];
    }

    // Constrói o vetor de saída ordenando pelos dígitos atuais
    for (int i = tamanho - 1; i >= 0; i--) 
    {
        saida[count[(vetor[i] / expoente) % 10] - 1] = vetor[i];
        count[(vetor[i] / expoente) % 10]--;
    }

    // Copia o resultado ordenado de volta para o vetor original
    for (int i = 0; i < tamanho; i++)
    {
        vetor[i] = saida[i];
    }

    delete[] saida; // libera memória auxiliar
}

// ============================================================
//                  FUNÇÃO PRINCIPAL RADIX SORT
// ============================================================
/*
    RadixSort: ordena um vetor de inteiros.

    Parâmetros:
        - vetor: ponteiro para o array de inteiros a ser ordenado
        - tamanho: número de elementos no array
 
    Funcionamento:
        - Encontra o maior valor do vetor para determinar o número de dígitos
        - Para cada posição decimal (expoente = 1, 10, 100, ...), chama CountingSort
          para ordenar os elementos de acordo com o dígito atual
        - Repete até que todos os dígitos do maior número tenham sido processados
 */
void RadixSort(int *vetor, int tamanho) 
{
    int max = vetor[0];
    // Encontra o maior elemento para saber quantos dígitos processar
    for (int i = 1; i < tamanho; i++)
    {
        if (vetor[i] > max)
        {
            max = vetor[i];
        }
    }

    // Ordena por cada dígito, da menor para a maior posição decimal
    for (int expoente = 1; max / expoente > 0; expoente *= 10)
    {
        CountingSort(vetor, tamanho, expoente);
    }
}

// ============================================================
//             FUNÇÃO DE EXECUÇÃO E MEDIÇÃO DE TEMPO
// ============================================================
/*
    ExecRadixSeq: executa o Radix Sort sequencial nos arquivos binários
    mede o tempo de ordenação e registra os resultados em CSV.
 
    Parâmetros:
        - entradas: array de caminhos (const char*) para arquivos binários
        - num_entradas: número de entradas no array
        - csv_saida: caminho do arquivo CSV de saída onde serão registrados os tempos

    Funcionamento:
        - Para cada arquivo:
        - Abre o arquivo e determina o número de inteiros
        - Lê os dados para um vetor alocado dinamicamente
        - Mede o tempo de ordenação usando chrono
        - Ordena os dados com RadixSort
        - Registra o tempo no arquivo CSV
        - Regrava o arquivo com os dados ordenados
        - Libera memória utilizada
 */
void ExecRadixSeq(const char **entradas, int num_entradas, const char *csv_saida)
{
    FILE *csv = fopen(csv_saida, "a");
    if (!csv)
    {
        perror("Erro ao abrir arquivo CSV");
        return;
    }

    for(int i = 0; i < num_entradas; i++)
    {
        FILE *file = fopen(entradas[i], "rb+");
        if(!file)
        {
            perror(entradas[i]);
            continue;
        }

        // Determina o número de inteiros no arquivo
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

        // Mede o tempo de ordenação usando chrono
        auto start = chrono::high_resolution_clock::now();
        RadixSort(v, tamanho);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        double tempo = elapsed.count();

        printf("Radix Sort Sequencial - Tempo para ordenar %s: %f segundos\n", entradas[i], tempo);

        // Registra tempo e tamanho no arquivo CSV
        fprintf(csv, "RadixSort Sequencial,%ld,%f\n", (size_t)tamanho, tempo);

        // Regrava o arquivo com os dados ordenados
        fseek(file, 0, SEEK_SET);
        if(fwrite(v, sizeof(int), tamanho, file) != tamanho)
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
}