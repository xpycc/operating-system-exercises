#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hw3_string.h"

#define MAXTESTS 100

struct test { 
 char name[100]; 
 int (*test)();
};

int test_create_and_extract() {
  char *hw = "Hello world\n";
  struct String *s = string_new(hw);
  char *str = string_chars(s);
  int result=strcmp(str,hw);
	if(totalAllocation() > 1000) { 
		fprintf(stderr,"ERROR: allocating too much memory.\n");
		return 0;
	}
  string_free(s);
  dfree(str);
  return result == 0;
}

int test_create_and_extract_large() {
  char *hw = "Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string. Hello world this is a very long string.\n";
  struct String *s = string_new(hw);
  char *str = string_chars(s);
  int result=strcmp(str,hw);
  string_free(s);
  dfree(str);
  return result == 0;
}

int test_clone_and_extract() {
  char *hw = "Hello world\n";
  struct String *s = string_new(hw);
  struct String *c = string_clone(s);
  char *str = string_chars(c);  
  int result=strcmp(str,hw);

  string_free(s);
  string_free(c);
  dfree(str);
  return result == 0;  
}

int test_char_at_index() {
  struct String *a=string_new("Hello world");
  struct String *b=string_new("Hello world!");  
  int result = string_charAtIndex(a,6)=='w' && 
         string_charAtIndex(b,0)=='H';
  string_free(a);
  string_free(b);

  return result;
}

int test_append() {
  struct String *c=string_new("Hello");
  string_append(c," world!");
	string_append(c," How");
	string_append(c," you");
	string_append(c," doin'?");

  int result = (string_charAtIndex(c,6)=='w') && 
		(string_charAtIndex(c,0)=='H') &&
		(string_charAtIndex(c,13)=='H') &&
		(string_charAtIndex(c,17)=='y') &&		
		(string_charAtIndex(c,19)=='u');

  string_free(c);
  return result;
}

int test_range() {
  struct String *s=string_new("Hello world");
  char *range=string_range(s,1,4);
  int result = strcmp("ello",range);
  string_free(s);
  dfree(range);
  return result == 0;
}

int test_range2() {
  struct String *s=string_new("Hel");
	string_append(s,"lo ");
	string_append(s,"wo");
	string_append(s,"rld");
  char *range=string_range(s,0,11);
  int result = strcmp("Hello world",range);
  string_free(s);
  dfree(range);
  return result == 0;
}

#define TEST(name){#name,&test_##name}


struct test tests[MAXTESTS]={
 TEST(create_and_extract),
 TEST(create_and_extract_large),
 TEST(clone_and_extract),
 TEST(char_at_index),
 TEST(range),
 TEST(append),
 TEST(range2)
};
#define NUM_TESTS 7

int main(int argc, char **argv) {
 int i;
 for(i=0;i<NUM_TESTS;i++) {   
   printf("Testing [%s]: ",tests[i].name);
   int pre_allocations = remainingAllocations();
   int result = tests[i].test();  
	 int post_allocations = remainingAllocations();
   if(!result) {
     printf("failed.\n"); exit(1);
   }
   else {
     printf("success.\n");
   }
   if(post_allocations>pre_allocations) {
    printf("WARNING: Leaked %d allocations.\n",post_allocations-pre_allocations);
    printAllocations();
   }
 }
 return 0;
}
