#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tokens_t parse_token(char token);
bool is_variable(char token);
void p_print_astNode_type(AstNode *n);
void p_print_token(tokens_t token);
void print_ast(AstNode *node);
void expect(char *expected, char received);
char peek(FILE *in);
AstNode *parse_expression(FILE *in, char token);
AstNode *parse_definition(FILE *in);
char *parse_variable(FILE *in, tokens_t token);
void free_ast(AstNode *node);
#endif
