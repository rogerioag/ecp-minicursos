verbatim:
	@g++ -O3 -mavx2 -fverbose-asm -S SomarDoisVetores128.cpp
	@g++ -O3 -mavx2 -fverbose-asm -S SomarDoisVetores256.cpp

compilar:
	@g++ -fno-tree-vectorize SomarDoisVetoresEscalares.cpp -o SomarDoisVetoresEscalar
	@g++ -mavx2 SomarDoisVetores256.cpp -o SomarDoisVetores256

clean:
	@rm -f *.s SomarDoisVetoresEscalar SomarDoisVetores256

