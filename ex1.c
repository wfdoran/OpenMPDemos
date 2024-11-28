/* 3.1 A Simple Parallel Loop
   https://www.openmp.org/wp-content/uploads/openmp-examples-6.0.pdf

   % gcc --version
   gcc (GCC) 14.1.0
   % gcc ex1.c -O2 -fopenmp -o ex1 -lm
   % setenv OMP_NUM_THREADS 1
   % ./ex1
   max_threads = 1 process time =   0.15313400 wall time   0.15369261
   % setenv OMP_NUM_THREADS 2
   % ./ex1
   max_threads = 2 process time =   0.15717800 wall time   0.07902190
   % setenv OMP_NUM_THREADS 4
   % ./ex1
   max_threads = 4 process time =   0.16274800 wall time   0.04104138
   % setenv OMP_NUM_THREADS 8
   % ./ex1
   max_threads = 8 process time =   0.17046700 wall time   0.02191720
   
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <omp.h>

void simple_loop(int n, float *a, float *b) {
# pragma omp parallel for
  for (int i = 1; i < n; i++) {
    b[i] = (a[i] + a[i - 1]) / 2.0;
  }
}

int main(void) {
  srand48(time(NULL));

  int n = 100000000;
  float *a = malloc(n * sizeof(float));
  assert(a != NULL);
  for (int i = 0; i < n; i++) {
    a[i] = i * drand48();
  }

  float *b = malloc(n * sizeof(float));
  assert(b != NULL);
  clock_t start_clock = clock();
  struct timespec start_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  simple_loop(n, a, b);
  clock_t end_clock = clock();
  struct timespec end_time;
  clock_gettime(CLOCK_MONOTONIC, &end_time);

  
  double proc_time = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
  double wall_time =
    (end_time.tv_sec + ((double) end_time.tv_nsec) / 1000000000) -
    (start_time.tv_sec + ((double) start_time.tv_nsec) / 1000000000);

  printf("max_threads = %d process time = %12.8f wall time %12.8f\n",
         omp_get_max_threads(), proc_time, wall_time);

  for (int i = 1; i < n; i++) {
    assert(fabs(b[i] - (a[i] + a[i - 1]) / 2.0) < 0.00000001);
  }
  
  free(a);
  free(b);
  return 0;
}
