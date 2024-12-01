/* 3.2 Parallel Construct
   https://www.openmp.org/wp-content/uploads/openmp-examples-6.0.pdf

   % gcc --version
   gcc (GCC) 14.1.0

   % gcc ex2.c -O2 -fopenmp -o ex2 -lm

   % setenv OMP_NUM_THREADS 2

   % ./ex2
   1   50000000  100000000
   0          0   50000000
   max_threads = 2 process time =   0.05907500 wall time   0.03181780

   % setenv OMP_NUM_THREADS 4

   % ./ex2
   0          0   25000000
   3   75000000  100000000
   1   25000000   50000000
   2   50000000   75000000
   max_threads = 4 process time =   0.06716300 wall time   0.01890891
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <omp.h>

#define VAL ((float) 123.45)
#define EPS (0.000001)

void subdomain(float *x, int start, int end) {
  for (int i = start; i < end; i++) {
    x[i] = VAL;
  }
}

void sub(float *x, int n) {
#pragma omp parallel
  {
    int my_th = omp_get_thread_num();
    int nth = omp_get_num_threads();
    int block_size = n / nth;
    int start = my_th * block_size;
    int end = my_th == nth - 1 ? n : (my_th + 1) * block_size;
    printf("%d %10d %10d\n", my_th, start, end); 
    subdomain(x, start, end);
  }
}

int main(void) {
  int n = 100000000;
  float *a = malloc(n * sizeof(float));
  assert(a != NULL);
  
  clock_t start_clock = clock();
  struct timespec start_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  sub(a, n);
  clock_t end_clock = clock();
  struct timespec end_time;
  clock_gettime(CLOCK_MONOTONIC, &end_time);

  
  double proc_time = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
  double wall_time =
    (end_time.tv_sec + ((double) end_time.tv_nsec) / 1000000000) -
    (start_time.tv_sec + ((double) start_time.tv_nsec) / 1000000000);

  printf("max_threads = %d process time = %12.8f wall time %12.8f\n",
         omp_get_max_threads(), proc_time, wall_time);

  for (int i = 0; i < n; i++) {
    if (fabs(a[i] - VAL) >= EPS) {
      printf("%d %8.4f %20.16f\n", i, a[i], fabs(a[i] - VAL));
    }
    assert(fabs(a[i] - VAL) < EPS);
  }
  
  free(a);
  return 0;
}
