import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("results/tempos.csv")

plt.figure(figsize=(10,6))

for algoritmo in df["Algoritmo"].unique():
    dados = df[df["Algoritmo"] == algoritmo].sort_values(by="Tamanho")
    plt.plot(dados["Tamanho"], dados["Tempo"], marker="o", label=algoritmo)

plt.xscale("symlog")
plt.xlabel("Tamanho da entrada (n)")
plt.ylabel("Tempo de execução (s)")
plt.title("Comparação de Desempenho dos Algoritmos de Ordenação")
plt.legend()
plt.grid(True, which="both", linestyle="--", alpha=0.6)

plt.tight_layout()
plt.savefig("results/grafico_comparacao.png", dpi=300)
plt.show()
