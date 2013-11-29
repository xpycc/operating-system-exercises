#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* top;
// guaranteed to be called only once
void init_mymalloc() {
	top=sbrk(0);
}

void* mymalloc(int size) {
	int chunksize = sizeof(int) + size + sizeof(int);
	if(chunksize < 16) chunksize = 16;
	
	if(top+chunksize > sbrk(0)) {
		if((int)sbrk(chunksize)==-1) {
			perror("In mymalloc(), calling sbrk.");
			return 0;
		}
	}
	char* ptr=top;
	top+=chunksize;
	return ptr+sizeof(int);
}

void myfree(void* alloc) {

}
