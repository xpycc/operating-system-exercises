#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dmalloc.h"
#include "hw3_string.h"

struct String {
  char *str;
};

struct String *string_new(char* str) {
  struct String* newString = (struct String*)dmalloc(sizeof(struct String));
  newString->str = (char*)dmalloc(strlen(str)+1);
  strcpy(newString->str, str);
  return newString;
}

struct String *string_clone(struct String* s) {
  struct String* newString = (struct String*)dmalloc(sizeof(struct String));
  newString->str = (char*)dmalloc(strlen(s->str)+1);
  strcpy(newString->str, s->str);
  return newString;
}

void string_free(struct String* s) {
	dfree(s->str);
  dfree(s);
}

int string_length(struct String* s) {
  return strlen(s->str);
}

void string_append(struct String* a, char *b) {
  int alen = string_length(a);
  int blen = strlen(b);

  char *newBuf = (char*)dmalloc(alen+blen+1);
  strcpy(newBuf,a->str);
  strcpy(newBuf+alen,b);   // overwrites the null terminator of the first copy
	dfree(a->str);
	a->str=newBuf;
}

char string_charAtIndex(struct String* s,  int index) {
  return s->str[index];
}

char* string_range(struct String* s, int start,  int length) {
  char *substr = (char*)dmalloc(length+1);
  memcpy(substr,s->str+start,length);
  substr[length]=0; // null terminator
  return substr;
}

char* string_chars(struct String* s) {
  char *c=dmalloc(string_length(s)+1);
  strcpy(c,s->str);
  return c;
}

