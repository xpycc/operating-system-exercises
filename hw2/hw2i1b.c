#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct LargeStruct {
  char largearray[4096*1024];
};

/*
struct LargeStruct
incrementCharAtOffset(struct LargeStruct s, int offset)
{
  s.largearray[offset]++;
  return s;
}
*/

void incrementCharAtOffset(struct LargeStruct *s, int offset)
{
  s->largearray[offset]++;
}

struct LargeStruct myLargeStruct;

int main() {
  int incrementOffset = 10;
  printf("Value at offset %d before incrementing %d\n",
      incrementOffset,
      myLargeStruct.largearray[incrementOffset]);

  // myLargeStruct=incrementCharAtOffset(myLargeStruct,incrementOffset);
  incrementCharAtOffset(&myLargeStruct, incrementOffset);

  printf("Value at offset %d after incrementing %d\n",
      incrementOffset,
      myLargeStruct.largearray[incrementOffset]);
  return 0;
}
