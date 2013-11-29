#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(int argc, char** argv) {
  if (argc != 2) { fprintf(stderr, "Usage: %s <index>\n", argv[0]); exit(1); }
  int n = atoi(argv[1]), i, j;
  assert(2 <= n && n < 75);
  for (i = n; i >= 3; --i) {
    long long a = 1, b = 1;
    for (j = 2; j < i; ++j) {
      long long c = a + b;
      b = a;
      a = c;
    }
    printf("%lld,", a);
  }
  puts("1,1");
  return 0;
}
