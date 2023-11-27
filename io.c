#include "io.h"
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

void delete_file(char *path) {
  int status = remove(path);
  if (status != 0) {
    printf("Error: Could not delete file %s\n", path);
    exit(1);
  }
}

void close_file(FILE *file) {
    fclose(file);
}
