#ifndef PARSER_H
#define PARSER_H

#include "io.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  L_PAREN,
  R_PAREN,
  LAMBDA,
  DOT,
  VARIABLE,
  ERROR,
  WHITESPACE,
} tokens_t;

typedef struct AstNode AstNode;

typedef enum {
  LAMBDA_EXPR,
  APPLICATION,
  VAR,
} AstNodeType;

typedef struct {
  char *parameter;
  AstNode *body;
} LambdaExpression;

typedef struct {
  AstNode *function;
  AstNode *argument;
} Application;

typedef struct {
  char *name;
} Variable;

typedef union {
  LambdaExpression *lambda_expr;
  Application *application;
  Variable *variable;
} AstNodeUnion;

struct AstNode {
  AstNodeType type;
  AstNodeUnion node;
};

tokens_t parse_token(char token);
char *parse_variable(FILE *in, tokens_t token);
bool is_variable(char token);
void p_print_astNode_type(AstNode *n);
void p_print_token(tokens_t token);
void print_ast(AstNode *node);
void expect(char *expected, char received);
char peek(FILE *in);
AstNode *parse_expression(FILE *in, char token);
void free_ast(AstNode *node);
#endif
