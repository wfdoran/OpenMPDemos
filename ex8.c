/* 3.9 linear clause 

% gcc -O2 -fopenmp ex8.c -o ex8

*/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
  int n = 1000000;

  int *a = malloc(n * sizeof(int));
  int *b0 = malloc((n / 2) * sizeof(int));
  int *b1 = malloc((n / 2) * sizeof(int));

  a[0] = 1;
  for (int i = 1; i < n ; i++) {
    a[i] = (3 * a[i-1]) % 9999823;
  }
  
  int j = 0;
#pragma omp parallel
#pragma omp for linear(j:1)
  for (int i = 0; i < n; i += 2) {
    b0[j++] = a[i] + 3;
  }

  for (int i = 0, j = 0; i < n; i += 2, j++) {
    b1[j] = a[i] + 3;
  }
  
  for (int i = 0; i < n/2; i++) {
    assert(b0[i] == b1[i]);
  }


  free(a);
  free(b0);
  free(b1);
  return 0;
}

  
