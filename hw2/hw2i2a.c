#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// char reversed[1024*1024]="Huge";

int main(int argc, char** argv) {
  if(argc != 2) { fprintf(stderr,"Usage: hw2i2a <word>\n"); exit(1); }

  char *reversed = malloc(strlen(argv[1] + 1));
  memset(reversed,0,sizeof(reversed));
  int i;
  for(i=0;i<strlen(argv[1]);i++) {
    reversed[i]=argv[1][strlen(argv[1])-i-1];
  }

  printf("%s = reversed(%s)\n",reversed,argv[1]);
  printf("The reversed string array is %ld bytes.\n",sizeof(reversed));

  free(reversed);
}
