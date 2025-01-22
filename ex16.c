/* 4.1.1  Spread Affinity Policy
   4.1.2  Close Affinity Policy


   % setenv OMP_PLACES "{0:2}:8:2"
   % gcc -O2 -fopenmp ex16.c -o ex16
   % ./ex16
   2 8
   3 12
   0 0
   1 4
   ================
   0 0
   2 4
   1 2
   3 6

   Notice distribution of cpus/cores with spread vs close.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <omp.h>


void whoami(void) {
  int th_id = omp_get_thread_num();
  int cpu_id = sched_getcpu();

  printf("%d %d\n", th_id, cpu_id);
}


int main(void) {
  int num_threads = 4;
  
#pragma omp parallel proc_bind(spread) num_threads(num_threads)
  {
    whoami();
  }

  printf("================\n");

#pragma omp parallel proc_bind(close) num_threads(num_threads)
  {
    whoami();
  }

  
  return 0;
}
