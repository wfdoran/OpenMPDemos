/* 5.2 Task Priority


 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void compute_entry (float *x, int n) {
  *x = ((float) n)  * ((float) n);
}

void compute_array(float *a, int n) {
  #pragma omp parallel
  #pragma omp single
  {
    for (int i = 0; i < n; i++) {
#pragma omp task priority(i)
      compute_entry(&a[i], i+1);
    }
  }

}

int main(void) {
  int n = 100;
  float *a = malloc(n * sizeof(float));

  compute_array(a, n);

  float sum = 0.0;
  for (int i = 0; i < n; i++) {
    sum += a[i];
  }
  printf("%12.2f %12d\n", sum, n * (n+1) * (2*n+1) / 6);

  free(a);
  return 0;
}
