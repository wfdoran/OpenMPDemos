/* 3.10 parallel sections

   % gcc -O2 -fopenmp ex9.c -o ex9
   % ./ex9
*/

#include <stdio.h>
#include <time.h>

void foo(int n, int v) {
  struct timespec t = {
    .tv_sec = 0,
    .tv_nsec = 1000000,
  };
    
  for (int i = 0; i < n; i++) {
    printf("%d %d\n", i, v);
    nanosleep(&t, NULL);
  }
}

int main(void) {
  int n = 10;

#pragma omp parallel sections
  {
#pragma omp section
    foo(n, 1);

#pragma omp section
    foo(n, 2);

#pragma omp section
    foo(n, 3);
  }

  return 0;
}

    
  
