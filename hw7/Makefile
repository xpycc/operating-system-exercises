all: hw7

hw7: main.c hw7.c
# the -m32 forces gcc to compile a 32-bit binary
	gcc -m32 --std=gnu99 -g -o hw7 main.c hw7.c 

clean:
	rm -rf *~ *.dSYM hw7
