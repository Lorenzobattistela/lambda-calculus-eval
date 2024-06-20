#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "hash-table/hash_table.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IMPORT_HEADER "# DEF #"

tokens_t parse_token(char token);
bool is_variable(char token);
void p_print_astNode_type(AstNode *n);
void p_print_token(tokens_t token);
void print_ast(AstNode *node);
void expect(char *expected, char received);
char peek(FILE *in);
AstNode *parse_expression(HashTable *table, FILE *in, char token);
void parse_definition(HashTable *table, FILE *in);
char *parse_variable(FILE *in, tokens_t token);
void free_ast(AstNode *node);
AstNode *create_variable(char *name);
AstNode *create_application(AstNode *function, AstNode *argument);
AstNode *create_lambda(char *variable, AstNode *body);

#endif
