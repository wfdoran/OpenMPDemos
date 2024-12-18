/* 3.14 masked construct

  % gcc -O2 -fopenmp ex12.c -o ex12 -lm
  % ./ex12
   
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void masked_example(float *x, float *y, int n, double tol) {
  int num_big;
  int num_iters = 0;
  
#pragma omp parallel
  {
    do {
#pragma omp for
      for (int i = 0; i < n; i++) {
        y[i] = x[i];
      }
      
#pragma omp single
      {
        num_big = 0;
      }
      
#pragma omp for reduction(+:num_big)
      for (int i = 1; i < n-1; i++) {
        x[i] = (y[i-1] + y[i] + y[i+1]) / 3.0;
        double error = x[i] - y[i];
        if (fabs(error) > tol) {
          num_big++;
        }
      }
      
#pragma omp masked
      {
        num_iters++;
        printf("%d: num_iters = %d num_big = %d\n",
               omp_get_thread_num(), num_iters, num_big);
      }
    } while (num_big > 0);
    #pragma omp barrier

#pragma omp masked filter(1)
    {
      printf("%d: num_iters = %d\n", omp_get_thread_num(),
             num_iters);
    }
  } 
}


int main(void) {
  srand48(time(NULL));

  const int n = 1000000;
  float *x = malloc(n * sizeof(float));
  float *y = malloc(n * sizeof(float));

  for (int i = 0; i < n; i++) {
    x[i] = 100 * drand48();
  }

  const float tol = 0.01;

  
  masked_example(x, y, n, tol);

  free(x);
  free(y);
  return 0;
}
