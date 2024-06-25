#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Type {
  AstNode *expr;
  char *type;
  char *return_type;
} Type;

typedef struct TypeEnv {
  Type *type;
  struct TypeEnv *next;
} TypeEnv;

Type *typecheck(AstNode *expr, TypeEnv *env);
bool type_equal(Type *a, Type *b);
void add_to_env(TypeEnv **env, Type *type);
Type *lookup_type(TypeEnv *env, AstNode *expr);
Type *parse_function_type(char *type);
void assert(bool expr, char *error_msg);

#endif
