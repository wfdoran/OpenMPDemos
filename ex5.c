/* 3.5 Interaction Between the num_threads1 Clause and omp_set_dynamic

   % gcc ex5.c -O2 -fopenmp -o ex5 -lm

   On desktop with 4 procs
   % ./ex5
   3 of 10
   0 of 10
   2 of 10
   5 of 10
   1 of 10
   4 of 10
   7 of 10
   8 of 10
   6 of 10
   9 of 10
   ===================================
   num_procs = 4
   0 of 4
   3 of 4
   2 of 4
   1 of 4
   ===================================
   
   
   On SMP with 144 procs
   % ./ex5
   7 of 10
   9 of 10
   1 of 10
   5 of 10
   0 of 10
   4 of 10
   3 of 10
   8 of 10
   6 of 10
   2 of 10
   ===================================
   num_procs = 144
   1 of 10
   8 of 10
   4 of 10
   9 of 10
   0 of 10
   7 of 10
   5 of 10
   3 of 10
   2 of 10
   6 of 10
   ===================================
*/

#include <stdio.h>
#include <omp.h>

void foo(int num_threads, int dynamic) {
  omp_set_dynamic(dynamic);
#pragma omp parallel num_threads(num_threads)
  {
    printf("%d of %d\n", omp_get_thread_num(), omp_get_num_threads());
  }
#pragma omp barrier

  printf("===================================\n");
}

int main(void) {
  /* Without dynamic, will launch 10 threads. */
  foo(10, 0);

  /* With dynamic, will not over subscribe. */
  printf("num_procs = %d\n", omp_get_num_procs());
  foo(10, 1);
  
  return 0;
}

