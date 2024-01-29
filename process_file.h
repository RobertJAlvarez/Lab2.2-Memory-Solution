#ifndef __PROCESS_FILE_H__
#define __PROCESS_FILE_H__

typedef struct {
  char **lines;
  size_t lines_size;
  size_t n_lines;
} arr_str;

void free_char_pp(char **pp);

size_t read_file(FILE *file, char ***arr_lines);

#endif
