// 5.5 taskgroup construct

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <omp.h>

extern void start_background_work(void);
extern void print_results(void);

typedef struct node_t {
  int depth;
  uint64_t value;
  struct node_t *left;
  struct node_t *right;
} node_t;


uint64_t g_value = 0;

/* Generates a biased tree. */
void node_expand(node_t *n) {
  double max_depth = 15.0;
  double accept = 1.0 - n->depth / max_depth;

  if (drand48() < accept) {
    node_t *left = malloc(sizeof(node_t));
    left->depth = n->depth + 1;
    left->value = g_value;
    g_value++;
    left->left = NULL;
    left->right = NULL;
    node_expand(left);
    n->left = left;
  }

  if (drand48() < accept) {
    node_t *right = malloc(sizeof(node_t));
    right->depth = n->depth + 1;
    right->value = g_value;
    g_value++;
    right->left = NULL;
    right->right = NULL;
    node_expand(right);
    n->right = right;
  }
}

#define max_steps 100

uint64_t g_depth[max_steps];

void print_results() {
  for (int32_t i = 0; i < max_steps; i++) {
    printf("%4d %8ld\n", i, g_depth[i]);
  }
}

void check_step(uint64_t target) {
  if (g_depth[target] == -1) {
    printf("Search failed for %ld\n", target);
  }
}

void start_background_work(void) {
  printf("%s: thread = %d\n", __func__, omp_get_thread_num());
  sleep(5);
}


void compute_something(node_t *node, uint64_t target)
{
  if (node->value == target) {
    g_depth[target] = node->depth;
    printf("%s: tareget = %ld found by %d\n", __func__, target, omp_get_thread_num());
  }
}


uint64_t compute_tree(node_t *node, uint64_t target)
{
  // printf("AAA %d %d %d\n", target, node->depth, node->value);
  if (node->left)
    {
#pragma omp task
      compute_tree(node->left, target);
    }
  if (node->right)
    {
#pragma omp task
      compute_tree(node->right, target);
    }
#pragma omp task
  compute_something(node, target);
}



int main()
{
  node_t *root = malloc(sizeof(node_t));
  root->depth = 0;
  root->value = 0;
  root->left = NULL;
  root->right = NULL;

  node_expand(root);

  for (int32_t i = 0; i < max_steps; i++) {
    g_depth[i] = -1;
  }
  
#pragma omp parallel
#pragma omp single
  {
#pragma omp task
    start_background_work();
    for (int32_t i = 0; i < max_steps; i++)
      {
#pragma omp taskgroup
        {
#pragma omp task
          compute_tree(root, i);
        } 
        check_step(i);
      }
  } // only now is background work required to be complete
  print_results();
  return 0;
}
