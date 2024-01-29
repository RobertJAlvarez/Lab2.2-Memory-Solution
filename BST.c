#include <stdio.h>   //ssize_t, getline()
#include <stdlib.h>  //free(), malloc(), realloc()

#include "BST.h"
#include "my_string.h"
#include "process_file.h"
#include "tokenizer.h"

typedef enum { NO, YES } free_lines_t;

static size_t __arr_len(char **arr) {
  size_t n = 0;
  for (char **p = arr; *p; ++p) ++n;
  return n;
}

static void __free_bst_nodes(node_t *node, const free_lines_t free_node_brand) {
  if (node == NULL) return;

  __free_bst_nodes(node->left_child, free_node_brand);
  __free_bst_nodes(node->right_child, free_node_brand);

  if (free_node_brand == YES) free(node->brand);  // Free line

  free(node->models);
  free(node->left_child);
  free(node->right_child);
}

void free_bst(bst_t *bst) {
  __free_bst_nodes(bst->root, YES);
  free(bst->root);
  free(bst);
}

static int __add_to_node(node_t *node, char *line_models) {
  char *p;
  size_t n_node_models;

  // Change node string '\0' for ','
  n_node_models = __arr_len(node->models);
  for (size_t i = 0; i < n_node_models; ++i) {
    *(node->models[i] - 1 * sizeof(char)) = ',';
  }

  // line_models starts at the first M in "B'\0'M1,..."
  //  change '\0' to ',' and point to it
  line_models -= 1 * sizeof(char);
  *line_models = ',';

  // Make space for brand and all models
  p = (char *)realloc(
      node->brand,
      (str_len(node->brand) + str_len(line_models) + 1) * sizeof(char));
  if (p == NULL) return 1;

  // Update brand
  node->brand = str_sep(&p, ",");

  // Append line_models to end of p and update node models
  free(node->models);
  node->models = tokenize(str_cat(p, line_models), ",");

  return 0;
}

static node_t *__create_node(char *line_brand, char *line_models) {
  node_t *node;

  if ((node = (node_t *)malloc(sizeof(node_t))) == NULL) return NULL;

  node->brand = line_brand;
  if ((node->models = tokenize(line_models, ",")) == NULL) return NULL;
  node->left_child = NULL;
  node->right_child = NULL;

  return node;
}

int add_line_to_bst(bst_t *bst, char *line) {
  char *line_brand;
  node_t *curr;
  node_t **new_node_p = NULL;
  int comp;

  curr = bst->root;
  line_brand = str_sep(&line, ",");

  // if root is empty, add node as root
  if (curr == NULL) {
    if ((bst->root = __create_node(line_brand, line)) == NULL) return 1;
    return 0;
  }

  // Otherwise, iterate bst to find the correct child
  while (curr != NULL) {
    comp = str_cmp(curr->brand, line_brand);
    if (comp == 0) {
      return __add_to_node(curr, line);
    }
    new_node_p = (comp > 0 ? &curr->left_child : &curr->right_child);
    curr = *new_node_p;
  }

  if ((*new_node_p = __create_node(line_brand, line)) == NULL) return 1;

  return 0;
}

static int __add_lines_to_bst(bst_t *bst, char **lines, size_t high) {
  size_t mid = high / 2;

  if (add_line_to_bst(bst, lines[mid]) != 0) return 1;

  if (mid != 0)
    if (__add_lines_to_bst(bst, lines, mid - 1) != 0) return 1;

  if (high != 0)
    if (__add_lines_to_bst(bst, &lines[mid + 1], high - mid - 1) != 0) return 1;

  return 0;
}

bst_t *create_bst(FILE *file) {
  bst_t *bst;
  char **lines = NULL;
  size_t n_lines;

  if ((bst = (bst_t *)malloc(sizeof(bst_t))) == NULL) return NULL;

  // Process file so there is only one brand per line and all
  //  models of that brand following it
  if ((n_lines = read_file(file, &lines)) == 0) return NULL;

  // Perform binary search in lines to create balance BST
  if (__add_lines_to_bst(bst, lines, n_lines - 1) != 0) {
    // bst nodes are pointing to the pointers in lines. So,
    //  char * would be free in free_bst()
    free_char_pp(lines);
    __free_bst_nodes(bst->root, /* free_lines =*/NO);
    free(bst->root);
    free(bst);
    bst = NULL;
  }

  // Don't free individual char * because a node is pointing
  //  to it
  free(lines);

  return bst;
}

/* Print menu options */
void print_menu(void) {
  printf("These are the inventory options:\n");
  printf("1: Write BST to file.\n");
  printf("2: Insert new line with format 'BRAND,MODEL1,...,MODELN' to bst.\n");
  printf("3: Print BST to standard output.\n");
  printf("4: Calculate tree height.\n");
  printf("5: Calculate number of nodes in bst.\n");
  printf("6. Exit.\n");

  return;
}

static void __write_node(const node_t *node, FILE *fp) {
  char **p;
  char *prev;

  if (node == NULL) return;

  __write_node(node->left_child, fp);

  // Print node info as BRAND,MODEL1,MODEL2,...,MODELN
  fprintf(fp, "%s,", node->brand);

  prev = *node->models;

  for (p = &node->models[1]; *p; ++p) {
    fprintf(fp, "%s,", prev);
    prev = *p;
  }
  fprintf(fp, "%s\n", prev);

  __write_node(node->right_child, fp);

  return;
}

void write_bst(const bst_t *bst, FILE *file) { __write_node(bst->root, file); }

void print_bst(const bst_t *bst) { __write_node(bst->root, stdout); }

static int __node_height(const node_t *node) {
  int left_height;
  int right_height;

  if (node == NULL) return 0;

  left_height = __node_height(node->left_child);
  right_height = __node_height(node->right_child);

  return 1 + (left_height > right_height ? left_height : right_height);
}

int bst_height(const bst_t *bst) { return __node_height(bst->root); }

static size_t __count_nodes(const node_t *node) {
  if (node == NULL) return 0;
  return 1 + __count_nodes(node->left_child) + __count_nodes(node->right_child);
}

size_t number_of_nodes(const bst_t *bst) { return __count_nodes(bst->root); }
