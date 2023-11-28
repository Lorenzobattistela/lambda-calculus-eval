#ifndef IO_H
#define IO_H
#include <stdio.h>

FILE *create_file(char *path);
FILE *get_file(char *path, char *mode);
void delete_file(char *path);
void close_file(FILE *file);
char next(FILE *file);

#endif
