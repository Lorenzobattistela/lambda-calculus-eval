#ifndef COMMON_H
#define COMMON_H

#define HANDLE_NULL(ptr)                                                       \
  do {                                                                         \
    if ((ptr) == NULL) {                                                       \
      fprintf(stderr, "ERROR: Null pointer encountered at %s:%d in %s\n",      \
              __FILE__, __LINE__, __func__);                                   \
      abort();                                                                 \
    }                                                                          \
  } while (0)

typedef enum {
  L_PAREN,
  R_PAREN,
  LAMBDA,
  DOT,
  VARIABLE,
  ERROR,
  WHITESPACE,
  NEWLINE,
  EQ,
} tokens_t;

typedef struct AstNode AstNode;

typedef enum {
  LAMBDA_EXPR,
  APPLICATION,
  VAR,
  DEFINITION,
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

#endif
