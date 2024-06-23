#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Type environment
typedef struct TypeEnv {
    char *var_name;
    char *type;
    struct TypeEnv *next;
} TypeEnv;

// Function prototypes
char *check_type(AstNode *node, TypeEnv *env);
char *check_lambda(LambdaExpression *lambda, TypeEnv *env);
char *check_application(Application *app, TypeEnv *env);
char *check_variable(Variable *var, TypeEnv *env);
void add_to_env(TypeEnv **env, char *var_name, char *type);
char *lookup_type(TypeEnv *env, char *var_name);


#endif
