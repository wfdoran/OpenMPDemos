/* 3.17 Using omp_set_dynamic and omp_set_num_threads.  */

#include <omp.h>
#include <stdlib.h>
#include <assert.h>

int main(void)
{
  int n = 16;
  
  omp_set_dynamic(0);
  omp_set_num_threads(n);

  int a[n];
  
#pragma omp parallel
  {
    if (omp_get_num_threads() != 16) {
      abort();
    }
    
    int th_id = omp_get_thread_num();
    a[th_id] = th_id * th_id;
  }

  for (int i = 0; i < n; i++) {
    assert(a[i] == i * i);
  }
  return 0;
}
