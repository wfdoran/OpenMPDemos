/* 3.3 Teams Construct

   % gcc ex3.c -O2 -fopenmp -o ex3 -lm
   % ./ex3
   AAA 0 of 2
   BBB team: 0 of 2, thread: 0 of 2
   BBB team: 0 of 2, thread: 1 of 2
   AAA 1 of 2 
   CCC team: 1 of 2, thread: 1 of 2
   CCC team: 1 of 2, thread: 0 of 2
   i=999999 sp|dp 999999.0000000000 999999.0099999900 
   i=500000 sp|dp 500000.0000000000 500000.0050000000 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


int main(void) {

  int n = 1000000;
  float *sp_x = malloc(n * sizeof(float));
  float *sp_y = malloc(n * sizeof(float));
  double *dp_x = malloc(n * sizeof(double));
  double *dp_y = malloc(n * sizeof(double));

  const float sp_a = 0.0001;
  const double dp_a = 0.0001;

  int nteams_required=2; 
  int max_thrds = omp_get_num_procs() / nteams_required;

  // Create 2 teams, each team works in a different precision
#pragma omp teams num_teams(nteams_required) thread_limit(max_thrds)
  {
    int tm_id = omp_get_team_num();
    int num_teams = omp_get_num_teams();
    printf("AAA team: %d of %d\n", tm_id, num_teams);
    
    if (num_teams != 2) { 
      printf("error: Insufficient teams on host, 2 required\n");
      exit(0);
    }

    // Do Single Precision Work (SAXPY) with this team
    if (tm_id == 0) { 
#pragma omp parallel
      {
        printf("BBB team: %d of %d, thread: %d of %d\n", tm_id, num_teams,
               omp_get_thread_num(), omp_get_num_threads());
#pragma omp for 
        for(int i=0; i<n; i++){
          sp_x[i] = i * 0.0001;
          sp_y[i] = i;
        }
        
#pragma omp for simd simdlen(8)
        for(int i=0; i<n; i++){
          sp_x[i] = sp_a * sp_x[i] + sp_y[i];
        }
      }
    }
    
    // Do Double Precision Work (DAXPY) with this team 
    if (tm_id == 1) {    
#pragma omp parallel
      {
        printf("CCC team: %d of %d, thread: %d of %d\n", tm_id, num_teams,
               omp_get_thread_num(), omp_get_num_threads());
#pragma omp for 
        for(int i=0; i<n; i++) {
          dp_x[i] = i * 0.0001;
          dp_y[i] = i;
        }
        
#pragma omp for simd simdlen(4)
        for(int i=0; i<n; i++){
          dp_x[i] = dp_a * dp_x[i] + dp_y[i];
        }
      }
    }
  }
  
  printf("i=%d sp|dp %.10f %.10f \n", n-1, sp_x[n-1], dp_x[n-1]);
  printf("i=%d sp|dp %.10f %.10f \n", n/2, sp_x[n/2], dp_x[n/2]);

  free(sp_x);
  free(sp_y);
  free(dp_x);
  free(dp_y);
  return 0;
}
