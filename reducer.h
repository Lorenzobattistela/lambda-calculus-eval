#ifndef REDUCER_H
#define REDUCER_H

#include "common.h"
#include "hash-table/hash_table.h"
#define SIZE 122
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AstNode *reduce(HashTable *table, AstNode *n);
AstNode *reduce_ast(HashTable *table, AstNode *n);
void expand_definitions(HashTable *table, AstNode *n);
AstNode *deepcopy(AstNode *n);
AstNode *deepcopy_application(AstNode *function, AstNode *argument);
AstNode *deepcopy_lambda_expr(char *parameter, AstNode *body, char *type);
AstNode *deepcopy_var(char *name, char *type);
AstNode *substitute(AstNode *expression, char *variable, AstNode *replacement);
void replace(AstNode *n, char *old, char *new_name);
void p_print_ast(AstNode *node);

#endif
