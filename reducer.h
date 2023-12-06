#ifndef REDUCER_H
#define REDUCER_H

#include "parser.h"

struct AstNode *reduce_ast(struct AstNode *n);
struct AstNode *deepcopy(struct AstNode *n);
struct AstNode *deepcopy_application(struct AstNode *function, struct AstNode *argument);
struct AstNode *deepcopy_lambda_expr(char parameter, struct AstNode *body);
struct AstNode *deepcopy_var(char name);
struct AstNode *substitute(struct AstNode *expression, char variable, struct AstNode *replacement);

#endif
