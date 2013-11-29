#include "hw7.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_ALLOCATIONS 10000
unsigned char* allocs[MAX_ALLOCATIONS];

int timediff() {
	static struct timeval before;
	struct timeval now;
	gettimeofday(&now,0);
	int diff = (now.tv_sec - before.tv_sec)*1000 + (now.tv_usec - before.tv_usec)/1000;
	before = now;
	return diff;
}

int check_consistency(int check_all) {
	
	// check for consistency. first write numbers to every allocated byte
	for(int i=0;i<MAX_ALLOCATIONS-1;i++) 
		if(check_all) {
			if(allocs[i]) {
				for(int j=0;j<i*2;j++) 
					allocs[i][j]=i%256;
			}
			else {
				fprintf(stdout,"Inconsistency: allocs[%d] is null\n",i);
				exit(1);
			}
		}
	
	// now check that every number is correct
	for(int i=0;i<MAX_ALLOCATIONS-1;i++) 
		if(check_all) {
			if(allocs[i]) {
				for(int j=0;j<i*2;j++) 
					if(allocs[i][j] != i%256) {
						fprintf(stdout,"Inconsistency: allocs[%d][%d]!=%d, instead %d\n",i,j,i%256,allocs[i][j]);
						exit(1);
					}
			}
			else {
				fprintf(stdout,"Inconsistency: allocs[%d] is null\n",i);
				exit(1);
			}
		}
	
}


int check_consistency2(int count, int size) {	

	// check for consistency. first write numbers to every allocated byte
	for(int i=0;i<count;i++) 
		for(int j=0;j<size;j++) 
			allocs[i][j]=i%256;
	
	// now check that every number is correct
	for(int i=0;i<count;i++) 
		for(int j=0;j<size;j++) 
			if(allocs[i][j] != i%256) {
				fprintf(stdout,"Inconsistency: allocs[%d][%d]!=%d, instead %d\n",i,j,i%256,allocs[i][j]);
				exit(1);
			}
}

int main(int argc, char** argv) {
		 if(argc<2) {
			 fprintf(stderr,"Usage: hw7 <random_seed>\n");
			 exit(1);
		 }
		 init_mymalloc();

		 timediff();
		 srandom(atoi(argv[1]));
		 
		 /* simple alloc / free test below */

		 void *pre = sbrk(0);
		 for(int i=0;i<MAX_ALLOCATIONS;i++) 
			 allocs[i]=mymalloc(i*2); 		 
		 check_consistency(1);
		 for(int i=0;i<MAX_ALLOCATIONS;i++) 
			 myfree(allocs[i]);

		 for(int i=0;i<MAX_ALLOCATIONS;i++) 
			 allocs[i]=mymalloc(i*2); 		 
		 check_consistency(1);
		 for(int i=0;i<MAX_ALLOCATIONS;i++) {
			 myfree(allocs[i]);
			 allocs[i]=0;
		 }		 
		 printf("Simple alloc/free, alloc/free cycle: %d, timediff:%d\n",sbrk(0)-pre,timediff());

		 /* coalescing free and splitting malloc test below */

		 for(int i=0;i<MAX_ALLOCATIONS;i++) 
			 allocs[i]=mymalloc(1024);

		 check_consistency2(MAX_ALLOCATIONS,1024);

		 // free all but one
		 for(int i=0;i<MAX_ALLOCATIONS-1;i++) {
			 myfree(allocs[i]);
			 allocs[i]=0;
		 }


		 pre=sbrk(0);
		 // alloc 9 chunks of 10k each, which should now end up in the coalesced region
		 for(int i=0;i<9;i++) 
			 allocs[i]=mymalloc(1000*1024);
		 printf("Coalescing free and splitting mymalloc: %d, timediff:%d\n",sbrk(0)-pre,timediff());
		 
		 check_consistency2(9,1000*1024);

		 for(int i=0;i<MAX_ALLOCATIONS;i++) 
			 if(allocs[i]) {
				 myfree(allocs[i]);
				 allocs[i]=0;
			 }
		 printf("After freeing coalesced blocks: %d, timediff:%d\n",sbrk(0)-pre,timediff());
		 

		 /* random alloc/free test below */

		 pre=sbrk(0);
		 for(int i=0;i<100000;i++) {			 
			 // maybe free a block
			 int free_index=random()%MAX_ALLOCATIONS;
			 if(allocs[free_index]) {
				 myfree(allocs[free_index]);
				 allocs[free_index]=0;
			 }

			 // maybe allocate a block
			 int alloc_index=random()%MAX_ALLOCATIONS;
			 if(!allocs[alloc_index])
						allocs[alloc_index]=mymalloc(2*alloc_index); 
		 }
		 
		 check_consistency(0);

		 // free remaining MAX_ALLOCATIONS
		 for(int i=0;i<MAX_ALLOCATIONS;i++) {
			 if(allocs[i]) {
				 myfree(allocs[i]);
				 allocs[i]=0;
			 }
		 }

		 printf("Random malloc and free sequence: %d, timediff: %d\n",sbrk(0)-pre,timediff());
		 pre=sbrk(0);
		 
}
