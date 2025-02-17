/*  5.1 task clause

    gcc -O2 -fopenmp ex18.c -o ex18




 */

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <omp.h>
#include <unistd.h>

typedef struct node_t {
  int depth;
  uint64_t value;
  struct node_t *left;
  struct node_t *right;
} node_t;

/* Generates a biased tree. */
void node_expand(node_t *n) {
  double max_depth = 10.0;
  double accept = 1.0 - n->depth / max_depth;

  if (drand48() < accept) {
    node_t *left = malloc(sizeof(node_t));
    left->depth = n->depth + 1;
    left->value = 3 * n->value + 1;
    left->left = NULL;
    left->right = NULL;
    node_expand(left);
    n->left = left;
  }

  if (drand48() < accept) {
    node_t *right = malloc(sizeof(node_t));
    right->depth = n->depth + 1;
    right->value = 3 * n->value + 2;
    right->left = NULL;
    right->right = NULL;
    node_expand(right);
    n->right = right;
  }
}

void node_destroy(node_t *n) {
  if (n->left) {
    node_destroy(n->left);
  }
  if (n->right) {
    node_destroy(n->right);
  }
  free(n);
}

int node_count(node_t *n) {
  int left_count = 0;
  int right_count = 0;

  if (n->left) 
    left_count = node_count(n->left);

  if (n->right)
    right_count = node_count(n->right);

  struct timespec req = {0, 10000000};
  nanosleep(&req, NULL);
  return 1 + left_count + right_count;
}

int *thread_dist;
int omp_count;

void node_count_omp(node_t *n) {
  int my_thread = omp_get_thread_num();

#pragma omp atomic
  thread_dist[my_thread] += 1;

#pragma omp atomic
  omp_count++;
  
#pragma omp task
  {
    if (n->left) {
      node_count_omp(n->left);
    }
  }

#pragma omp task
  {
    if (n->right) {
      node_count_omp(n->right);
    }
  }

  struct timespec req = {0, 10000000};
  nanosleep(&req, NULL);
  return;
}

int main(void) {
  srand48(time(NULL));

  int num_threads = omp_get_max_threads();
  thread_dist = malloc(num_threads * sizeof(int));
  for (int th = 0; th < num_threads; th++) {
    thread_dist[th] = 0;
  }
  
  node_t *root = malloc(sizeof(node_t));
  root->depth = 0;
  root->value = 0;
  root->left = NULL;
  root->right = NULL;

  node_expand(root);

  {
    clock_t start_clock = clock();
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    int count = node_count(root);
    clock_t end_clock = clock();
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    
    printf("node_count = %d\n", count);
    
    double proc_time = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
    double wall_time =
      (end_time.tv_sec + ((double) end_time.tv_nsec) / 1000000000) -
      (start_time.tv_sec + ((double) start_time.tv_nsec) / 1000000000);
    
    printf("max_threads = %d process time = %12.8f wall time %12.8f\n",
           omp_get_max_threads(), proc_time, wall_time);
  }

  {
    clock_t start_clock = clock();
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    omp_count = 0;
#pragma omp parallel
#pragma omp single
    node_count_omp(root);
#pragma omp taskwait
    
    clock_t end_clock = clock();
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    
    printf("node_count = %d\n", omp_count);
    
    double proc_time = ((double) (end_clock - start_clock)) / CLOCKS_PER_SEC;
    double wall_time =
      (end_time.tv_sec + ((double) end_time.tv_nsec) / 1000000000) -
      (start_time.tv_sec + ((double) start_time.tv_nsec) / 1000000000);
    
    printf("max_threads = %d process time = %12.8f wall time %12.8f\n",
           omp_get_max_threads(), proc_time, wall_time);
  }
    

  node_destroy(root);

  printf("\n");
  printf("thread count\n");
  for (int th = 0; th < num_threads; th++) {
    if (thread_dist[th] > 0) {
      printf("%3d %8d\n", th, thread_dist[th]);
    }
  }
  free(thread_dist);

  return 0;
}
