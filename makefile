all: MIPS_interpreter

MIPS_interpreter: MIPS_interpreter.cpp
	g++ "MIPS_interpreter.cpp" -o "MIPS_interpreter" -std=c++17 -Wall -Wextra

run: all
	./MIPS_interpreter < in.asm > out
