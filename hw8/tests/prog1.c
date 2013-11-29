
#include <stdio.h>

int k=10;
char * mystring="hello friend";

void anotherFunction(int arg1, int arg2){
  printf("hello world %d %d\n", arg1, arg2);
}

int main(int argc, char ** argv){

	int sum=0;
	int i=0;
	for (; i < 10; ++i){
		sum+=10;
	}
	anotherFunction(5,10);
}
