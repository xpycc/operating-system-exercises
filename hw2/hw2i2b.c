#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char *concatenate(char* a, char* b) {
  int la = strlen(a), lb = strlen(b);
  char *r = malloc(la + lb + 1);
  int i;
  for (i = 0; i < la; ++i)
    r[i] = a[i];
  for (i = 0; i < lb; ++i)
    r[la + i] = b[i];
  r[la + lb] = 0;
  return r;
}

int main(int argc, char** argv) {
  if(argc<4) { printf("Usage: hw2i2b <count> <firstword> <secondword>\n");
    exit(1); }

  char *middle="cruel";
  char number[10];
  int i;

  for(i=0;i<atoi(argv[1]);i++) {
    sprintf(number,"%d",i); // update the number string

    // begin leaky
    char *line = concatenate( // not leaked
        concatenate(argv[2],number), // leaked
        concatenate(middle, // leaked
          concatenate(number, argv[3]))); // leaked
    if(i>0)  // can't free the statically allocated "cruel" string above
      free(middle); 
    // end leaky      
    printf("%s\n",line);
    middle=line;
  }
  free(middle);
  return 0;
}
