/* 3.11 firstprivate clause


% gcc -O2 -fopenmp ex10.c -o ex10   
% ./ex10
BBB count = 1
AAA count = 1
CCC count = 0
================
AAA count = 11
BBB count = 11
CCC count = 10
================
AAA count = 1
BBB count = 1
CCC count = 10
================
BBB count = 1
AAA count = 1
CCC count = 1
================


 */


#include <omp.h>
#include <stdio.h>

// firstprivate: in each section count is private but initialized to the
// global value.
int version1(int init_value) {
  int count = init_value;

#pragma omp parallel
#pragma omp sections firstprivate(count)
  {
#pragma omp section
    {
      count++;
      printf("AAA count = %d\n", count);
    }

#pragma omp section
    {
      count++;
      printf("BBB count = %d\n", count);
    }
  }

  printf("CCC count = %d\n", count);
  printf("================\n");
}

// private: in each section count is private but uninitialized (typically gets 0)
// The current value of count does not matter
int version2(int init_value) {
  int count = init_value;

#pragma omp parallel
#pragma omp sections private(count)
  {
#pragma omp section
    {
      count++;
      printf("AAA count = %d\n", count);
    }

#pragma omp section
    {
      count++;
      printf("BBB count = %d\n", count);
    }
  }

  printf("CCC count = %d\n", count);
  printf("================\n");
}


// No clause.  Both threads update the same variable.  Can get 1/2/2, 2/1/2, or even 1/1/1. 
int version3(int init_value) {
  int count = init_value;

#pragma omp parallel
#pragma omp sections
  {
#pragma omp section
    {
      count++;
      printf("AAA count = %d\n", count);
    }

#pragma omp section
    {
      count++;
      printf("BBB count = %d\n", count);
    }
  }

  printf("CCC count = %d\n", count);
  printf("================\n");
}

int main(void) {
  version1(0);
  version1(10);
  version2(10);
  version3(0);
  
  return 0;
}
