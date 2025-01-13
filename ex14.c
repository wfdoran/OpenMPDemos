/*  3.16 Parallel Random Access Iterator Loop

  % gcc -O2 -fopenmp ex14.c -o ex14 -lm
  % setenv OMP_NUM_THREADS 1
  % ./ex14
  1000000 
  max_threads = 1 process time =   0.00056200 wall time   0.00055244
  % setenv OMP_NUM_THREADS 2
  % ./ex14
  500000 500000 
  max_threads = 2 process time =   0.00224700 wall time   0.00134741
  % setenv OMP_NUM_THREADS 4
  % ./ex14
  250000 250000 250000 250000 
  max_threads = 4 process time =   0.00468700 wall time   0.00234967
 
*/ 

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <omp.h>

int main(void) {
  int n = 1000000;
  int *x = malloc(n * sizeof(int));
  assert(x != NULL);
  
  int *end = &x[n];

  for (int i = 0; i < n; i++) {
    x[i] = i;
  }

  clock_t start_clock = clock();
  struct timespec start_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  int nth = omp_get_max_threads();
  int count[nth];
  for (int i = 0; i < nth; i++) {
    count[i] = 0;
  }
  
#pragma omp parallel for
  for (int *y = x; y < end; y++) {
    int v = *y;
    *y = v+10;
    count[omp_get_thread_num()]++;
  }
  clock_t end_clock = clock();
  struct timespec end_time;
  clock_gettime(CLOCK_MONOTONIC, &end_time);

  for (int i = 0; i < nth; i++) {
    printf("%d ", count[i]);
  }
  printf("\n");
  
  double proc_time = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
  double wall_time =
    (end_time.tv_sec + ((double) end_time.tv_nsec) / 1000000000) -
    (start_time.tv_sec + ((double) start_time.tv_nsec) / 1000000000);

  printf("max_threads = %d process time = %12.8f wall time %12.8f\n",
         omp_get_max_threads(), proc_time, wall_time);
   

  for (int i = 0; i < n; i++) {
    assert(x[i] == i + 10);
  }

  free(x);

  return 0;
}
