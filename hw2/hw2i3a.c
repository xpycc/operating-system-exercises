#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

long long f(int x, long long *prev, char **os) {
  assert(2 <= x && x < 75);
  if (x == 2) {
    *prev = 1;
    *os = malloc(4);
    (*os)[0] = '1'; (*os)[1] = ','; (*os)[2] = '1'; (*os)[3] = '\0';
    return 1;
  }
  long long pr; char *ps;
  *prev = f(x - 1, &pr, &ps);
  *os = malloc(25 + strlen(ps));
  sprintf(*os, "%lld,", *prev + pr);
  int len = strlen(*os), i;
  for (i = 0; ps[i]; ++i)
    (*os)[len + i] = ps[i];
  free(ps);
  (*os)[len + i] = '\0';
  return *prev + pr;
}

int main(int argc, char** argv) {
  if (argc != 2) { fprintf(stderr, "Usage: %s <index>\n", argv[0]); exit(1); }
  long long x; char *out;
  f(atoi(argv[1]), &x, &out);
  puts(out); free(out);
  return 0;
}
