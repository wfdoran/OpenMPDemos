/* 3.15 Loop Construct




 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <omp.h>

void simple_loop(float *x, float *y, int n) {

  const float a = 2.0;

#pragma omp parallel
  {
#pragma omp loop
    for (int i = 0; i < n; i++) {
      y[i] = a * x[i] + y[i];
    }
  }
}

int main(void) {
  
  const int n = 1000000;
  float *x = malloc(n * sizeof(float));
  assert(x != NULL);
  float *y = malloc(n * sizeof(float));
  assert(y != NULL);

  for (int i = 0; i < n; i++) {
    x[i] = (float) i;
    y[i] = 0.0;
  }

  clock_t start_clock = clock();
  struct timespec start_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);
 
  simple_loop(x, y, n);
  
  clock_t end_clock = clock();
  struct timespec end_time;
  clock_gettime(CLOCK_MONOTONIC, &end_time);

  const double eps = 0.00001;
  for (int i = 0; i < n; i++) {
    assert(fabs(y[i] - 2.0 * i) < eps);
  }

  double proc_time = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
  double wall_time =
    (end_time.tv_sec + ((double) end_time.tv_nsec) / 1000000000) -
    (start_time.tv_sec + ((double) start_time.tv_nsec) / 1000000000);

  printf("max_threads = %d process time = %12.8f wall time %12.8f\n",
         omp_get_max_threads(), proc_time, wall_time);

  
  free(x);
  free(y);
  return 0;
}
