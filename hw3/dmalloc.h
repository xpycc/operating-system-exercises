
#ifdef DEBUG_MALLOC
#define dmalloc(size) _dmalloc(size,__FILE__,__LINE__)
#define dfree(ptr) _dfree(ptr,__FILE__,__LINE__)
#else
#define dmalloc(size) calloc(1,size)
#define dfree(ptr) free(ptr)
#endif


void* _dmalloc(size_t size, char* file, int line);
void _dfree(void* ptr, char* file, int line);

int remainingAllocations();
int totalAllocation();
void printAllocations();
