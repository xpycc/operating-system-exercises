#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "hw3_string.h"

char indicators[5]="-\\|/";

int main(int argc, char **argv) {
  if(argc<2) { fprintf(stderr,"Usage: speedtests <file>\n"); exit(1);}
  FILE* f = fopen(argv[1],"r");

  char line[100];
  int linecount=0;

	struct timeval before,after;

	gettimeofday(&before,0);
	printf("Reading book: -");
  fflush(stdout);
	struct String *book=string_new("");
  while(fgets(line,100,f)) {
		string_append(book,line);
		printf("\b%c",indicators[(linecount++/100) % 4]);
 		fflush(stdout);
  }  
  gettimeofday(&after,0);
  printf("\n+ReadIn: %0.3lf sec\n",(after.tv_sec*1000000.0+after.tv_usec-before.tv_sec*1000000.0-before.tv_usec)/1000000.0);

  printf("Found characters: ");
  fflush(stdout);
	gettimeofday(&before,0);
  unsigned length = string_length(book);
  for(int i=0;i<100000;i++) {    
		char c = string_charAtIndex(book,(i*774735)%length);
		if(i%2000==0 && c!='\r' && c!='\t' && c!='\n' && c!=' ') printf("%c",c);
	}
	gettimeofday(&after,0);
	printf("\nExcerpts: \n'%s'\n'%s'\n'%s'\n",string_range(book,0,73),string_range(book,10009,40),string_range(book,1000028,50));
  printf("+Index time: %0.6lf sec\n",(after.tv_sec*1000000.0+after.tv_usec-before.tv_sec*1000000.0-before.tv_usec)/1000000.0);
 	string_free(book);	

	/* start all over again */
	fclose(f);
	f=fopen(argv[1],"r");
	linecount=0;

	gettimeofday(&before,0);
	printf("Read-in and index combined: -");
  fflush(stdout);
	book=string_new("");
  while(fgets(line,100,f)) {
		string_append(book,line);		
		printf("\b%c",indicators[(linecount++/100) % 4]);
		unsigned length = string_length(book);
		string_charAtIndex(book,(linecount*774735)%length);
 		fflush(stdout);
  }  
  gettimeofday(&after,0);
  printf("\n+Combined: %0.3lf sec\n",(after.tv_sec*1000000.0+after.tv_usec-before.tv_sec*1000000.0-before.tv_usec)/1000000.0);

  return 0;
}
