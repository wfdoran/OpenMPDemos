/* 3.8 collapse clause


  % gcc -O2 -fopenmp ex7.c -o ex7
  % setenv OMP_NUM_THREADS 4
  % ./ex7
     collapse: process time =   0.07059100 wall time   0.07981138
  no collapse: process time =   0.11825700 wall time   0.12237158
  % setenv OMP_NUM_THREADS 8
  %  ./ex7
     collapse: process time =   0.06761300 wall time   0.07276435
  no collapse: process time =   0.11907600 wall time   0.12680368

 */

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#define N (500)


void sub_collapse(float *a) {

#pragma omp for collapse(2)
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      for (int k = 0; k < N; k++) {
        a[i * N * N + j * N + k] = 0.1 * i * i + 0.3 * j * 1.1 * k;
      }
    }
  }
}

void sub_no_collapse(float *a) {

#pragma omp for
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      for (int k = 0; k < N; k++) {
        a[i * N * N + j * N + k] = 0.1 * i * i + 0.3 * j * 1.1 * k;
      }
    }
  }
}

int main(void) {
  float *a = malloc(N * N * N * sizeof(float));
  float *b = malloc(N * N * N * sizeof(float));

  {
    clock_t start_clock = clock();
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    sub_collapse(a);
    clock_t end_clock = clock();
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double proc_time = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
    double wall_time =
      (end_time.tv_sec + ((double) end_time.tv_nsec) / 1000000000) -
      (start_time.tv_sec + ((double) start_time.tv_nsec) / 1000000000);
    printf("   collapse: process time = %12.8f wall time %12.8f\n",
           proc_time, wall_time);

  }
  
#pragma omp barrier
  
  {
    clock_t start_clock = clock();
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    sub_no_collapse(b);
    clock_t end_clock = clock();
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double proc_time = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
    double wall_time =
      (end_time.tv_sec + ((double) end_time.tv_nsec) / 1000000000) -
      (start_time.tv_sec + ((double) start_time.tv_nsec) / 1000000000);
    printf("no collapse: process time = %12.8f wall time %12.8f\n",
           proc_time, wall_time);

  }
  
#pragma omp barrier
  
  float eps = 0.0001;
  for (int i = 0; i < N * N * N; i++) {
    assert(fabs(a[i] - b[i]) < eps);
  }

  free(a);
  free(b);
  return 0;
}
