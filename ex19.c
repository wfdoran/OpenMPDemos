/*  5.1 task clause: fibonacci example

    gcc -O2 -fopenmp ex19.c -o ex19




 */

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <omp.h>
#include <unistd.h>


int fib(int n) {
  if (n < 2) {
    return n;
  }

  int i = fib(n - 1);
  int j = fib(n - 2);
  return i + j;
}




int *thread_dist;


int fib_omp(int n) {
  int my_thread = omp_get_thread_num();

#pragma omp atomic
  thread_dist[my_thread] += 1;

  if (n < 2) {
    return n;
  }


  int i, j;
#pragma omp task shared(i)
  {
    i = fib_omp(n - 1);
  }

#pragma omp task shared(j)
  {
    j = fib_omp(n - 2);
  }

#pragma omp taskwait
  return i + j;
}

int main(void) {
  srand48(time(NULL));

  int num_threads = omp_get_max_threads();
  thread_dist = malloc(num_threads * sizeof(int));
  for (int th = 0; th < num_threads; th++) {
    thread_dist[th] = 0;
  }

  int n = 15;
  
  {
    clock_t start_clock = clock();
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    int f_out = fib(n);
    clock_t end_clock = clock();
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    
    printf("fib(%d) = %d\n", n, f_out);
    
    double proc_time = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
    double wall_time =
      (end_time.tv_sec + ((double) end_time.tv_nsec) / 1000000000) -
      (start_time.tv_sec + ((double) start_time.tv_nsec) / 1000000000);
    
    printf("max_threads = %d process time = %12.8f wall time %12.8f\n",
           omp_get_max_threads(), proc_time, wall_time);
  }

  {
    clock_t start_clock = clock();
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    int f_out;
#pragma omp parallel
#pragma omp single
    f_out = fib_omp(n);
#pragma omp taskwait
    
    clock_t end_clock = clock();
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    printf("fib(%d) = %d\n", n, f_out);

    double proc_time = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
    double wall_time =
      (end_time.tv_sec + ((double) end_time.tv_nsec) / 1000000000) -
      (start_time.tv_sec + ((double) start_time.tv_nsec) / 1000000000);
    
    printf("max_threads = %d process time = %12.8f wall time %12.8f\n",
           omp_get_max_threads(), proc_time, wall_time);
  }

  printf("\n");
  printf("thread count\n");
  for (int th = 0; th < num_threads; th++) {
    if (thread_dist[th] > 0) {
      printf("%3d %8d\n", th, thread_dist[th]);
    }
  }
  free(thread_dist);

  return 0;
}
