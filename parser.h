#ifndef PARSER_H
#define PARSER_H

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
} tokens_t;

enum AstNodeType {
    LAMBDA_EXPR,
    APPLICATION,
    VAR,
};

struct LambdaExpression;
struct Application;
struct Variable;

union AstNodeUnion {
    struct LambdaExpression* lambda_expr;
    struct Application* application;
    struct Variable* variable;
};

struct AstNode {
    enum AstNodeType type;
    union AstNodeUnion node;
};

tokens_t parse_token(char *token);
bool is_variable(char *token);
void p_print_token(tokens_t token);

#endif
