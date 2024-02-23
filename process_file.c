#include <stdio.h>   //FILE, fprintf()
#include <stdlib.h>  //size_t, malloc(), realloc(), free()

#include "my_string.h"
#include "process_file.h"
#include "util.h"

enum try_combine_brands { NO, YES };

static char **__make_null_terminated(arr_str *lines) {
  char **arr_lines;
  void *temp;

  arr_lines = lines->lines;

  if (lines->n_lines == lines->lines_size) {
    ++lines->lines_size;
    temp = realloc(lines->lines, (lines->lines_size + 1) * sizeof(char *));

    if (temp == NULL) return NULL;

    arr_lines = (char **)temp;
  }

  arr_lines[lines->lines_size] = NULL;

  return arr_lines;
}

static char *__remove_last_non_printable(char *s, size_t size) {
  --size;
  while (s[size] < ' ' || s[size] > '~') s[size--] = '\0';
  return s;
}

static int __add_line(arr_str *lines, char *line,
                      const enum try_combine_brands combine) {
  size_t line_size;
  char *line_copy;

  line_size = str_len(line);

  if (combine == YES && lines->n_lines > 0) {
    // If this line brand and previous line brand are the
    //  same then it must be combine
    char *prev_line;
    char *brand_end;

    prev_line = lines->lines[lines->n_lines - 1];

    // Get line brand
    brand_end = str_p_brk(line, ",");
    if (brand_end == NULL) return 1;

    // If the brands are the same, combine models in single
    //  char * variable, update lines and return
    if (str_n_cmp(prev_line, line, ((size_t) (brand_end - line))) == 0) {
      char *new_models;
      char *temp;

      new_models = &line[brand_end - line];
      temp = (char *)realloc(prev_line,
                             str_len(prev_line) + str_len(new_models) + 1);

      if (temp == NULL) return 1;
      // Combine models and update lines
      lines->lines[lines->n_lines - 1] = str_cat(temp, new_models);
      return 0;
    }
  }
  // else, line is added normally

  // Add more size to lines if needed
  if (lines->lines_size == lines->n_lines) {
    void *temp;

    lines->lines_size *= 2;
    temp = realloc(lines->lines, (lines->lines_size) * sizeof(char *));
    if (temp == NULL) return 1;
    lines->lines = (char **)temp;
  }

  // Make a copy of line and add it to lines
  line_copy = (char *)malloc(line_size * sizeof(char));
  if (line_copy == NULL) return 1;
  lines->lines[lines->n_lines++] = mem_cpy(line_copy, line, line_size);

  return 0;
}

static arr_str *__init_arr_str(void) {
  arr_str *lines;

  lines = (arr_str *)malloc(sizeof(arr_str));
  if (lines == NULL) return NULL;

  lines->lines_size = 8;
  lines->n_lines = 0;

  lines->lines = (char **)malloc(lines->lines_size * sizeof(char *));
  if (lines->lines == NULL) {
    free(lines);
    return NULL;
  }

  return lines;
}

void free_char_pp(char **pp) {
  for (char **p = pp; *p; ++p) free(*p);
  free(pp);
}

size_t read_file(FILE *file, char ***arr_lines) {
  arr_str *lines;
  char *buffer;          // Buffer to read each line
  size_t bufsize = 128;  // Number of characters that the buffer can store
  ssize_t n_chars;       // Number of characters read and store in buffer
  size_t n_lines;

  // Allocate memory
  buffer = (char *)malloc(bufsize * sizeof(char));
  if (buffer == NULL) {
    fprintf(stderr, "Could not allocate memory for buffer.\n");
    return 0;
  }

  lines = __init_arr_str();
  if (lines == NULL) {
    fprintf(stderr, "Could not allocate memory to store file lines\n");
    free(buffer);
    return 0;
  }

  // Read file and populate lines
  while ((n_chars = getline(&buffer, &bufsize, file)) > ((ssize_t)0)) {
    // Remove last non-printable characters from buffer, make
    //  it null terminated and add file line to lines
    if (__add_line(lines, __remove_last_non_printable(buffer, (size_t)n_chars),
                   /* try_combine =*/YES) != 0) {
      fprintf(stderr, "Could not add file line into lines read.\n");
      // Free buffer
      free(buffer);
      for (size_t i = 0; i < lines->n_lines; ++i) free(lines->lines[i]);
      free(lines->lines);
      free(lines);
      return 0;
    }
  }

  // No lines read
  if (lines->n_lines == 0) {
    free_char_pp(lines->lines);
    free(lines);
    free(buffer);

    return 0;
  }

  // Make lines->lines null terminated
  *arr_lines = __make_null_terminated(lines);

  n_lines = lines->n_lines;

  free(lines);
  free(buffer);

  return n_lines;
}
