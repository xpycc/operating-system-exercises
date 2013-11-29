#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dmalloc.h"
#include "hw3_string.h"

struct String {
  char *str;
  int len, cap;
};

struct String *string_new(char* str) {
  struct String* newString = (struct String*)dmalloc(sizeof(struct String));
  newString->len = strlen(str);
  newString->cap = newString->len == 0 ? 1 : newString->len;
  newString->str = (char*)dmalloc(newString->cap + 1);
  strcpy(newString->str, str);
  return newString;
}

struct String *string_clone(struct String* s) {
  struct String* newString = (struct String*)dmalloc(sizeof(struct String));
  newString->len = s->len;
  newString->cap = s->len;
  newString->str = (char*)dmalloc(s->len + 1);
  strcpy(newString->str, s->str);
  return newString;
}

void string_free(struct String* s) {
  dfree(s->str);
  dfree(s);
}

int string_length(struct String* s) {
  return s->len;
}

void string_append(struct String* a, char *b) {
  int alen = string_length(a);
  int blen = strlen(b);
  int ncap = a->cap;
  while (ncap < alen + blen)
    ncap <<= 1;

  if (a->cap < ncap) {
    char *newBuf = (char*)dmalloc(ncap + 1);
    strcpy(newBuf,a->str);
    dfree(a->str);
    a->str = newBuf;
  }
  strcpy(a->str + alen,b);
  a->len = alen + blen;
  a->cap = ncap;
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

