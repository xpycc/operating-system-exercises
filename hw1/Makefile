
BINARIES = hw1i2a hw1i2b hw1i2c hw1i2d
OBJECTCODE = parser.o hw1.o

all: $(BINARIES)

$(BINARIES): %: %.c $(OBJECTCODE)
	gcc -g -std=c99 -o $@ $^

$(OBJECTCODE): %.o: %.c %.h
	gcc -g -std=c99 -o $@ $< -c
	
clean:
	rm $(BINARIES) $(OBJECTCODE)
