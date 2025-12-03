FLAG = sm_75

all:
	mkdir -p dados
	mkdir -p results
	mkdir -p results/sequencial
	mkdir -p results/threads
	mkdir -p results/cuda
	nvcc -O3 -arch=$(FLAG) -std=c++14 -o main src/main.cu
	./main
	python3 plot_tempos.py
	rm -f dados/*.bin
	rm main


clean_dados:
	rm -f dados/*.bin


plot:
	python3 plot_tempos.py