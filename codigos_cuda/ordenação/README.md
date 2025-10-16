# Projeto de Comparação de Algoritmos de Ordenação Paralela (C++ / CUDA)

## Descrição do Projeto

Este projeto implementa e compara o desempenho de diferentes algoritmos de ordenação — **MergeSort** e **RadixSort** — em três abordagens distintas:

- **Sequencial:** Implementação tradicional em CPU 
- **Threads:** Paralelização utilizando *threads* POSIX 
- **CUDA:** Paralelização massiva em GPU NVIDIA

O objetivo é analisar o ganho de desempenho obtido com o uso de diferentes técnicas de paralelismo, comparando o tempo de execução entre CPU e GPU.

---

## Pré-requisitos

### Sistema e Compiladores

- **Sistema Operacional:** Linux (testado no Zorin OS / Ubuntu)
- **Compilador C++:** GCC 7+ com suporte a C++17
- **CUDA Toolkit:** NVIDIA GPU com driver e compilador `nvcc` instalado e configurado no PATH 

  Verifique se o compilador CUDA está configurado corretamente:
  ```bash
  nvcc --version
  nvidia-smi
  which nvcc
  ```

- **Python:** Python 3.6+ com as bibliotecas `pandas` e `matplotlib`
  ```bash
  python3 -m pip install --user pandas matplotlib
  ```

---

## Compilação e Execução

### Clonando o repositório

```bash
git clone https://github.com/Goosyx/Minicurso-Cuda-Codigos.git
cd Minicurso-Cuda-Codigos
```

---

### ⚠️ Ajuste de arquitetura CUDA

Antes de executar o projeto, verifique a **Compute Capability** da sua GPU e ajuste o campo `FLAG` no `Makefile` conforme necessário.

|               GPU / Família (exemplos)             |  Arquitetura  | Compute Capability (CC) |       Flag correta (`-arch=`)        |
| -------------------------------------------------- | ------------- | ----------------------- | ------------------------------------ |
| GeForce GTX 9xx — ex.: GTX 970 / 980               | Maxwell       | 5.2                     | `sm_52`                              |
| Tesla P100 (Pascal Datacenter)                     | Pascal        | 6.0                     | `sm_60`                              |
| GeForce GTX 10xx — ex.: GTX 1060 / 1080            | Pascal        | 6.1                     | `sm_61`                              |
| Titan V / Tesla V100                               | Volta         | 7.0                     | `sm_70`                              |
| GeForce RTX 20xx — ex.: RTX 2060 / 2080 / 2080 Ti  | Turing        | 7.5                     | `sm_75`                              |
| NVIDIA A100 / A30 (Datacenter Ampere)              | Ampere        | 8.0                     | `sm_80`                              |
| GeForce RTX 30xx — ex.: RTX 3060 / 3080 / 3090     | Ampere        | 8.6                     | `sm_86`                              |
| GeForce RTX 40xx — ex.: RTX 4070 / 4080 / 4090     | Ada Lovelace  | 8.9                     | `sm_89`                              |
| GeForce RTX 50xx — ex.: RTX 5070 / 5080 / 5090     | Blackwell     | 12.0                    | `sm_120`                             |

> Este projeto foi testado com a arquitetura `sm_75` (Turing). 
> Ajuste conforme sua GPU para evitar o erro: 
> `no kernel image is available for execution on the device`.

---

## Threads POSIX

Para a execução dos algoritmos que utilizam threads, certifique-se de que seu sistema suporta a criação de múltiplas threads.

Você pode modificar o número de threads no arquivo `main.cu`, alterando o valor da variável:

```cpp
const int num_threads = 8;
```

---

## Compilação e Execução

### Executar o projeto completo

```bash
make all
```

Esse comando irá:

- Compilar o código principal (`src/main.cu`);
- Gerar os arquivos binários de teste em `dados/`;
- Executar **TODOS** os algoritmos de ordenação;
- Verificar se os arquivos estão ordenados;
- Salvar os resultados de tempo em `results/tempos.csv`;
- Produzir os gráficos de desempenho.

---

## Resultados

- Os tempos de execução são salvos em:
  ```
  results/tempos.csv
  ```

- O gráfico comparativo de desempenho é salvo em:
  ```
  results/grafico_comparacao.png
  ```

O gráfico mostra a evolução do tempo de execução em função do tamanho das entradas para cada abordagem.

---

## Dicas e Observações

- Caso sua GPU apresente erro de compatibilidade, edite o campo `FLAG` no `Makefile` conforme sua arquitetura.
- Em máquinas com pouca memória, reduza o tamanho dos vetores no arquivo `main.cu` (vetor `tamanho_arquivos`).
- Para evitar repetição de dados aleatórios entre execuções, adicione no início do `main`:
  ```cpp
  srand((unsigned)time(NULL));
  ```
- Durante depuração, você pode comentar as linhas `rm` no `Makefile` para preservar os binários após a execução.

---

## Estrutura do Projeto

```
Minicurso-Cuda-Codigos/
├── dados/
│   ├── 1m.bin
│   ├── 2m500.bin
│   ├── 5m.bin
│   ├── 7m500.bin
│   ├── 10m.bin
│   ├── 25m.bin
│   ├── 50m.bin
│   ├── 100m.bin
│   ├── 250k.bin
│   ├── 500k.bin
│   └── 750k.bin
│
├── results/
│   ├── grafico_comparacao1.png
│   └── tempos.csv
│
├── src/
│   ├── ordenadores/
│   │   ├── cuda/
│   │   │   ├── merge_sort_cuda.cu
│   │   │   └── radix_sort_cuda.cu
│   │   │
│   │   ├── sequencial/
│   │   │   ├── merge_sort_seq.h
│   │   │   └── radix_sort_seq.h
│   │   │
│   │   └── threads/
│   │       ├── merge_sort_threads.h
│   │       └── radix_sort_threads.h
│   │
│   ├── utils/
│   │   └── utils.h
│   │
│   ├── gerador_dados.cpp
│   └── main.cu
│
├── .gitignore
├── Makefile
├── plot_tempos.py
└── README.md
```

---

## Licença

Este projeto foi desenvolvido para fins acadêmicos e educacionais.
Sinta-se livre para utilizar, modificar e distribuir com os devidos créditos.

---

## Autor

**Sérgio Murilo (Goosyx)**
Email: *(sergiomurilo-cardoso@hotmail.com)*
GitHub: [Goosyx](https://github.com/Goosyx)