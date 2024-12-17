/* 3.12 single construct

   % gcc ex11.c -O2 -fopenmp -o ex11
   % setenv OMP_NUM_THREADS (4)
   % ./ex11
   0: start...
   3: 1
   3: middle...
   2: 1
   0: 1
   1: 1
   0: 2
   0: end...
   0: 3
   3: 2
   3: 3
   1: 2
   1: 3
   2: 2
   2: 3
   
 */

#include <stdio.h>
#include <omp.h>

void foo(int x) {
  printf("%d: %d\n", omp_get_thread_num(), x);
}


void bar() {
#pragma omp parallel
  {
#pragma omp single
    printf("%d: start...\n", omp_get_thread_num());

    foo(1);

    // implicit barrier.  All threads will print 1 before any prints 2.
#pragma omp single 
    printf("%d: middle...\n", omp_get_thread_num());

    foo(2);

    // no barrier.  Some threads may print 3 before all the 2s are printed
#pragma omp single nowait
    printf("%d: end...\n", omp_get_thread_num());

    foo(3);
  }
}

int main(void) {
  bar();
  return 0;
}
