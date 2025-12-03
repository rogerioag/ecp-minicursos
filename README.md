# ecp-minicursos

Minicursos e Tutoriais do Projeto de Extensão Escola de Computação Paralela (ECP)

__Projeto de Extensão:__ Escola de Computação Paralela (ECP 2024)

__Registro UTFPR:__ 10503

[Texto do Projeto](docs/projeto_extensao_escola_de_computacao_paralela_ecp_utfpr_2024.pdf)

## Objetivo

O objetivo principal do projeto é preparar e ministrar minicursos sobre temas relacionados a Computação Paralela. Com o intuito de capacitar os participantes (membros da comunidade externa, profissionais e alunos dos cursos da UTFPR) para o desenvolvimento de aplicações paralelas e trabalharem com temas atuais de pesquisa na área. Proporcionando uma formação complementar em temas de Computação Paralela, bem como o estudo de tecnologias e de linguagens de programação voltadas à Computação Paralela e de Alto Desempenho em Sistemas Heterogêneos. Além de preparar os participantes para comporem equipes de competição para maratonas e desafios de computação paralela e colaborar na divulgação científica dos temas relacionados à Computação.

Esperamos com esse projeto dar continuidade na elaboração de materiais para os minicursos em complemento ao material já preparado nas edições de 2017 e 2018 da ECP e do projeto de extensão associado a cada uma delas.
Queremos ministrar esses minicursos aos alunos do curso de Bacharelado em Ciência da Computação e Técnico Integrado em Informática e a todos os membros da comunidade acadêmica e externa que se interessarem pelos assuntos relacionados à Computação Paralela.
Serão propostas ações de extensão intituladas como "Quarta-Paralela" vinculadas a este projeto para a realização de cada minicurso. Para eventuais minicursos ministrados em eventos externos a emissão de certificado fica na responsabilidade dos organizadores.
O público-alvo esperado é composto por alunos, funcionários da UTFPR e profissionais da comunidade externa que tenham interesse pelos assuntos abordados. Pretendemos atender 22 participantes em cada uma das turmas para a execução de cada minicurso.
Como produção de recursos educacionais para serem disponibilizados, pretendemos elaborar um conjunto de materiais e tutoriais que serão agrupados em uma edição de um livro texto __"Escola de Computação Paralela: Minicursos e Tutoriais"__. Os minicursos pretendidos estão listados:

| Minicurso | Responsável |
| ---- | ---- |
| Introdução à Computação Paralela e Programação Multithreading usando Pthreads | Willian Wallace |
| Introdução à Programação Paralela com `OpenMP` | Paulo Henrique |
| Uso das Diretivas de Compilação do padrão OpenACC | em aberto |
| Introdução  à  Computação Paralela em Sistemas Heterogêneos: Programação para GPUs com a Plataforma CUDA | Guilherme Saides Serbai e Sergio Murilo Cardoso Valentini |
| Introdução à Infraestrutura de Compilação do LLVM | João Victor Briganti |
| Introdução à Computação Heterogênea com OpenCL | em aberto |
| Introdução à Implementação de Modelos de Aprendizagem Profunda com pyTorch usando GPUs | Gabriel Lobato |
| Introdução ao uso de TensorFlow para Computação em GPUs para Implementação de Modelos de Aprendizagem Profunda | em aberto |
| Introdução à Modernização de Código | em aberto |
| Introdução à Computação Heterogênea com a Linguagem Julia | em aberto |
| Suporte a Aceleradores em Linguagens de Programação Modernas | Christopher Eduardo Zai |
| Introdução à Programação para Sistemas Embarcados com FreeRTOS | Luiz Gustavo Takeda |
| Introdução à Computação Quântica usando `Qiskit` | Willian Wallace |
| Introdução à Compilação Distribuída | Gustavo Reino |


## Desenvolvimento

## Orientações Gerais

Cada minicurso tem dois arquivos principais:

- <nome-do-minicurso>-resumo.qmd
- <nome-do-minicurso>.qmd

Por exemplo, exemplos do minicurso _Introdução à Computação Paralela e Programação Multithreading usando Pthreads_ terá os arquivos `pthreads-resumo.qmd` e o `pthreads.qmd`, o primeiro contem um resumo do minicurso e o segundo o texto completo do minicurso.

### Figuras

As figuras utilizadas em cada um dos materiais devem ser colocadas dentro do diretório `images/<nome-do-minicurso>`. No minicurso tomado como exemplo, teremos o diretório `images/pthreds`.

### Código dos Exemplos

O código dos exemplos apresentados no material deve estar na pasta `src/<nome-do-minicurso>`, por exemplo, exemplos do minicurso _Introdução à Computação Paralela e Programação Multithreading usando Pthreads_ devem estar no diretório `src/pthreads`, podendo cada exemplo estar em um subdiretório dentro de `src/pthreads`.

### Markdown

Utilizem os recursos do Markdown, em específico do quarto markdown <https://quarto.org/docs/authoring/markdown-basics.html>

Deem uma olhada no site <quarto.org>

### Reuniões

As reuniões tem acontecido online via [`Google Meet`](https://meet.google.com/ixf-iznf-eye).

#### Reunião (12/05/2025 - Tarde)

* Falamos sobre a estrutura do projeto e das ferramentas ([quarto](www.quarto.org) e markdown) a serem utilizadas. [__Presentes:__ Willian, Gabriel, Paulo e Guilherme]. William sugeriu incluirmos um Minicurso de Computação Quântica com [`Qiskit`](https://www.ibm.com/quantum/qiskit).