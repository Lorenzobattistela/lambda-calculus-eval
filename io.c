#include "io.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>

FILE *create_file(char *path) {
  FILE *file = fopen(path, "w");
  if (file == NULL) {
    printf("Error: Could not create file %s\n", path);
    exit(1);
  }
  return file;
}

FILE *get_file(char *path, char *mode) {
  FILE *file = fopen(path, mode);
  if (file == NULL) {
    printf("Error: Could not open file %s\n", path);
    exit(1);
  }
  return file;
}

void write_to_file(FILE *file, char *content) {
  HANDLE_NULL(file);
  if (fputs(content, file) == EOF) {
    fprintf(stderr, "ERROR: could not write to file at %s:%d\n", __FILE__,
            __LINE__);
    exit(EXIT_FAILURE);
  }
}

void delete_file(char *path) {
  int status = remove(path);
  if (status != 0) {
    fprintf(stderr, "ERROR: could not delete to file at %s:%d\n", __FILE__,
            __LINE__);
    exit(EXIT_FAILURE);
  }
}

void close_file(FILE *file) {
  if (fclose(file) != 0) {
    fprintf(stderr, "ERROR: Could not close file at %s:%d\n", __FILE__,
            __LINE__);
    exit(EXIT_FAILURE);
  }
}

char next(FILE *file) {
  int x = fgetc(file);
  return (char)x;
}
