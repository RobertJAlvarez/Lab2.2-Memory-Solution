#include <stdio.h>   //fprintf()
#include <stdlib.h>  //free()

#include "BST.h"

int main(int argc, char **argv) {
  FILE *file;
  const char add_line[] = "Acura,some1,some2";

  if (argc != 2) {
    fprintf(stderr, "Only one parameter was expected and %d were given.\n",
            argc - 1);
    return 1;
  }

  file = fopen(argv[1], "r");

  if (file == NULL) {
    fprintf(stderr, "Error opening file '%s'\n", argv[1]);
    return 1;
  }

  bst_t *bst = create_bst(file);

  printf("BST information:\n");
  print_bst(bst);

  printf("\nAdding line: \"%s\" to bst...\n", add_line);
  add_line_to_bst(bst, (char *)add_line);

  printf("\nBST information:\n");
  print_bst(bst);

  printf("\nStarting to free bst...\n");
  free_bst(bst);
  printf("Done!\n");

  fclose(file);

  return 0;
}
