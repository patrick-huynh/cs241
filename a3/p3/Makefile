run:
	g++ -g -std=c++17 -Wall asm.cc scanner.cc -o asm
	cs241.binasm < input.asm > expected.mips
	./asm < input.asm > output.mips