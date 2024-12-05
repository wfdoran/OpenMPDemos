/* 3.4 Nested Parallel Regions.

   % setenv OMP_NUM_THREADS 8
   % gcc ex4.c -O2 -fopenmp -o ex4 -lm
   % ./ex4

*/

#include <stdio.h>
#include <omp.h>

void foo(int dynamic, int nested) {
  omp_set_dynamic(dynamic);
  omp_set_nested(nested);

#pragma omp parallel
  {
    int out_thread_num = omp_get_thread_num();
    printf ("AAA:     %d of %d\n", out_thread_num, omp_get_num_threads());

#pragma omp parallel
    {
      printf ("BBB: %d - %d of %d\n", out_thread_num, omp_get_thread_num(), omp_get_num_threads());
      
#pragma omp single
      {
        /* Note: which thread actually executes is random-ish. */
        printf ("CCC: %d - %d of %d\n", out_thread_num, omp_get_thread_num(), omp_get_num_threads());
      }
    }
  }
  
#pragma omp barrier

  printf("===================================\n");
}

void bar() {
  omp_set_dynamic(0);
  omp_set_nested(1);

  int max_threads = omp_get_max_threads();
  int th1 = 1;
  while ((th1 + 1) * (th1 + 1) <= max_threads) {
    th1++;
  }
  int th2 = max_threads / th1;
  
#pragma omp parallel num_threads(th1)
  {
    int out_thread_num = omp_get_thread_num();
    printf ("AAA:     %d of %d\n", out_thread_num, omp_get_num_threads());

#pragma omp parallel num_threads(th2)
    {
      printf ("BBB: %d - %d of %d\n", out_thread_num, omp_get_thread_num(), omp_get_num_threads());
      
#pragma omp single
      {
        /* Note: which thread actually executes is random-ish. */
        printf ("CCC: %d - %d of %d\n", out_thread_num, omp_get_thread_num(), omp_get_num_threads());
      }
    }
  }

  
#pragma omp barrier

  printf("===================================\n");
}


int main (void) {
  /* Get OMP_NUM_THREADS at the first level.
     Get 1 thread at each nested level. */
  foo(0, 0);

  /* Get OMP_NUM_THREADS at the first level.
     Get OMP_NUM_THREADS at each nested level. */
  foo(0, 1);

  /* Runtime decides.  Looks like
     Get num procs at the first level
     Get num procs at the second level */
  foo(1, 1);

  /* manually divide up the threads. */
  bar();
  return 0;
}
