#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

long long f[100];

int main(int argc, char** argv) {
  if (argc != 2) { fprintf(stderr, "Usage: %s <index>\n", argv[0]); exit(1); }
  int n = atoi(argv[1]);
  assert(2 <= n && n < 75);
  f[1] = f[2] = 1;
  int i;
  for (i = 3; i < 75; ++i)
    f[i] = f[i - 1] + f[i - 2];
  for (i = n; i >= 3; --i)
    printf("%lld,", f[i]);
  puts("1,1");
  return 0;
}
