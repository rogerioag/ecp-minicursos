verbatim:
	@g++ -O0 -S SomarDoisVetoresEscalares.cpp
	@g++ -O3 -mavx2 -S SomarDoisVetores256.cpp

compilar:
	@g++ -O0 SomarDoisVetoresEscalares.cpp -o SomarDoisVetoresEscalar
	@g++ -mavx2 SomarDoisVetores256.cpp -o SomarDoisVetores256

clean:
	@rm -f *.s SomarDoisVetoresEscalar SomarDoisVetores256

