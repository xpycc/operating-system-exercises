#include<stdio.h>
#include<stdlib.h>

/*
void addfive(int *value) {
  *value=(*value)+5;
}
*/

int addfive(int value) {
  return value + 5;
}

int main(int argc, char **argv) {
  if(argc!=2) { printf("Usage: hw2i1a <integer>\n"); exit(1); }
  int times = atoi(argv[1]);
  int accumulator=0;
  while(times--) {
    printf("Accumulator = %d\n",accumulator);
    // addfive(&accumulator);
    accumulator = addfive(accumulator);
  }
  return 0;
}
