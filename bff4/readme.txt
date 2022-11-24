Brainfuck

	FILES

readme.txt		This file
!			File used for sparation code and input in DBFI
bfdebug.html		Web page brainfuck interpreter and debugger
bff4.c			Fast brainfuck interpreter
dbfi.b			Famous Daniel's self-interpreter (http://www.esolangs.org/wiki/DBFI)
hi123.b			BF program printing 'hello123'
long.b			BF program printing 202 (ascii) and taking longer time to run
slowbf.cpp		Very short C++ interpreter; no memory limit
stackbfi.c		C interpreter without static or heap memory allocation


	NOTES

All interpreters here assume DBFI brainfuck dialect.
The code and input are sparated by '!'. For example

++++++[->+++<]>[->++++<]>.+.,.!!

prints 'HI!'. H and I are generated and '!' is read by ',' instruction
which is the second exclamation mark. The same output is produced by

,.,.,.!HI!


	EXAMPLES

(UNIX)
	cat dbfi.b ! hi123.b | ./bff4

Equivalent in WINDOWS is
	copy dbfi.b + ! + hi123.b = tmp.b
	bff4.exe < tmp.b
	del tmp.b

	cat dbfi.b ! dbfi.b ! hi123.b  | ./bff4

	bff4 < long.b

	cat long.b | stackbfi.exe

	cat dbfi.b ! hi123.b | slowbf.exe



