all: MIPS_interpreter

MIPS_interpreter: MIPS_interpreter.cpp
	g++ MIPS_interpreter.cpp -o MIPS_interpreter -std=c++17

clean:
	rm MIPS_interpreter
