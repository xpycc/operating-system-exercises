#include<stdlib.h>
#include<stdio.h>

struct allocation {
     struct allocation *next;
		 void *ptr;     

     /* helpful debugging info below. 
        the file and line of code where the allocation was made, and the size. */
     char *file; 
     int  line;
     int size;
};
struct allocation* allocations;

void* _dmalloc(size_t size, char* file, int line) {
  struct allocation* newAlloc = (struct allocation*)calloc(1,sizeof(struct allocation));
  newAlloc->ptr = calloc(1,size);
  newAlloc->next = allocations;
  newAlloc->file = file;
  newAlloc->line = line;
  newAlloc->size = size;
  allocations = newAlloc;
  return newAlloc->ptr;
}

void _dfree(void* ptr, char* file, int line) {
  struct allocation* alloc = allocations;
  struct allocation** prevPtr = &allocations;
  while(alloc && alloc->ptr != ptr) {
    prevPtr = &alloc->next;
    alloc=alloc->next;
  }
  if(alloc) {
    free(ptr);
    *prevPtr = alloc->next;  // remove this allocation entry from our list
    free(alloc);
  }
  else {		  
	 fprintf(stderr,"PANIC: %s:%d tried to free unallocated memory.\n",file,line);
	 exit(1);
  }
}

int remainingAllocations() {
  int i=0;
  struct allocation *alloc = allocations;
  while(alloc) { i++; alloc=alloc->next; }
  return i;
}

int totalAllocation() {
  int sum=0;
  struct allocation *alloc = allocations;
  while(alloc) { sum+=alloc->size; alloc=alloc->next; }
  return sum;
}

void printAllocations() {
  struct allocation *alloc = allocations;
  while(alloc) {
			 printf("%d bytes allocated at %s:%d\n",alloc->size,alloc->file,alloc->line);
			 alloc=alloc->next; 
  }
}
