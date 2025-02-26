/* 5.3   Task Dependency

% gcc ex21.c -o ex21 -fopenmp
% ./ex21
x = 2
x = 2

*/

#include <stdio.h>
#include <unistd.h>

int main(void) {
  int x = 1;

  #pragma omp parallel
  #pragma omp single
  {
#pragma omp task shared(x) depend(out: x)
    {
      sleep(1);
      x = 2;
    }
    
#pragma omp task shared(x) depend(in: x)
    printf("x = %d\n", x);
  }
 
  #pragma omp parallel
  #pragma omp single
  {
#pragma omp task shared(x)
    {
      sleep(1);
      x = 3;
    }
#pragma omp task shared(x)
    printf("x = %d\n", x);
  }
 
  return 0;
}
