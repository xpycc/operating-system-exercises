#include "dmalloc.h"

struct String;
struct String* string_new(char* str);
void string_free(struct String* s);
struct String* string_clone(struct String* s);

int string_length(struct String* s);
void string_append(struct String* a, char* b); 
char string_charAtIndex(struct String* s,  int index);
char* string_range(struct String* s,  int start,  int length);
char* string_chars(struct String* s);


