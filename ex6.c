/* 3.7 nowait clause


   % gcc -O2 -fopenmp ex6.c -o ex6 -lm
   % setenv OMP_NUM_THREADS 4
   % ./ex6
   no wait: process time =   0.03519400 wall time   0.00951549
      wait: process time =   0.03749400 wall time   0.01265817
*/


#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

void nowait_example(int n, float *a, float *b, float *y, float *z) {
#pragma omp parallel
 {
#pragma omp for nowait
   for (int i = 1; i < n; i++)
     b[i] = (a[i] + a[i-1]) / 2.0;

#pragma omp for nowait
   for (int i = 0; i < n; i++)
     y[i] = sqrt(z[i]);
 }
}


void wait_example(int n, float *a, float *b, float *y, float *z) {
#pragma omp parallel
 {
#pragma omp for nowait
   for (int i = 1; i < n; i++)
     b[i] = (a[i] + a[i-1]) / 2.0;

#pragma omp for nowait
   for (int i = 0; i < n; i++)
     y[i] = sqrt(z[i]);
 }
}

int main(void) {
  int n = 10000000;
  float *a = malloc(n * sizeof(float));
  float *b1 = malloc(n * sizeof(float));
  float *b2 = malloc(n * sizeof(float));
  float *z = malloc(n * sizeof(float));
  float *y1 = malloc(n * sizeof(float));
  float *y2 = malloc(n * sizeof(float));
  
  for (int i = 0; i < n; i++) {
    a[i] = 2.5 * i;
    z[i] = 3.5 * i * i;
  }

  {
    clock_t start_clock = clock();
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    nowait_example(n, a, b1, z, y1);
    clock_t end_clock = clock();
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double proc_time = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
    double wall_time =
      (end_time.tv_sec + ((double) end_time.tv_nsec) / 1000000000) -
      (start_time.tv_sec + ((double) start_time.tv_nsec) / 1000000000);
    printf("no wait: process time = %12.8f wall time %12.8f\n",
           proc_time, wall_time);
  }

#pragma omp barrier

  {
    clock_t start_clock = clock();
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    wait_example(n, a, b2, z, y2);
    clock_t end_clock = clock();
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double proc_time = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
    double wall_time =
      (end_time.tv_sec + ((double) end_time.tv_nsec) / 1000000000) -
      (start_time.tv_sec + ((double) start_time.tv_nsec) / 1000000000);
    printf("   wait: process time = %12.8f wall time %12.8f\n",
           proc_time, wall_time);
  }
  
  float eps = 0.0001;
  for (int i = 0; i < n - 1; i++) {
    assert(fabs(b1[i] - b2[i]) < eps);
    assert(fabs(y1[i] = y2[i]) < eps);
  }
  

  free(a);
  free(b1);
  free(b2);
  free(z);
  free(y1);
  free(y2);
  return 0;
}
