STRING_CODE=fast_string.c

BINARIES=unit speed
COMMON=$(STRING_CODE) dmalloc.c
CCOPT=-g -std=c99

all: $(BINARIES) 

unit: unittests.c $(COMMON)
	gcc $(CCOPT) -D DEBUG_MALLOC -o $@ unittests.c $(COMMON) 

speed: speed.c 
	gcc $(CCOPT) -o speed speed.c $(COMMON)

testunit: unit
	@if egrep -n "[^a-zA-Z_]free|malloc" *.c *.h | egrep -v dfree\|dmalloc; then\
	 echo "ERROR: malloc/free check failed. Use dmalloc/dfree instead.";\
        else\
	 ./unit;\
        fi         

testspeed: speed
	@if [ ! -e 2554.txt ]; then \
	echo "Downloading 'Crime and Punishment' by Fyodor Dostoevsky";\
        wget -q http://www.gutenberg.org/files/2554/2554.txt; fi; \
	./speed 2554.txt

clean:
	@rm -rf $(BINARIES) *.o *~ *.dSYM a.out 2554.txt
