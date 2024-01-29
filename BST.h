#ifndef __BST_FROM_FILE__
#define __BST_FROM_FILE__

typedef struct __node_t {
  char *brand;
  char **models;
  struct __node_t *left_child;
  struct __node_t *right_child;
  // You can add more fields as needed, but cannot modify
  //  already existent ones
} node_t;

typedef struct {
  node_t *root;
} bst_t;

/* Free everything from heap memory in bst */
void free_bst(bst_t *bst);

/* Given a string with the format "BRAND,MODEL1,...
 * MODELn\n", add to existing or create a node in bst to save
 * it.
 */
int add_line_to_bst(bst_t *bst, char *line);

/* Function that reads file where each line have the format
 * "BRAND,MODEL1,...,MODELn\n" and the lines are sorted in
 * ascending order with respect to "BRAND". Given that the
 * lines are sorted, the bst_t to be return
 * must be a balance bst.
 */
bst_t *create_bst(FILE *file);

/* Print menu options */
void print_menu(void);

/* Save bst given in the file given on the format "BRAND
 * MODEL1,...,MODELn" where the lines are sorted in ascending
 * order with respect to "BRAND".
 */
void write_bst(const bst_t *bst, FILE *file);

/* Print bst to standard output in ascending order on the
 * format "BRAND: MODEL1, MODEL2, ..., MODELn\n".
 */
void print_bst(const bst_t *bst);

/* Return the bst depth */
int depth(const bst_t *bst);

/* Return number of nodes in bst */
size_t number_of_nodes(const bst_t *bst);

#endif
