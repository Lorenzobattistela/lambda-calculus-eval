#include "io.h"
#include "parser.h"
#include "reducer.h"
#include <stdio.h>

void print_ast(struct AstNode *node);

int main(void) {
  FILE *in = get_file("expr.lambda", "r");
  
  struct AstNode *res = parse_expression(in, next(in)); 
  print_ast(res);
  printf("\nTesting deepcopy: \n");
  struct AstNode *res_2 = (struct AstNode *)malloc(sizeof(struct AstNode));
  res_2->type = LAMBDA_EXPR;
  res_2->node.lambda_expr = (struct LambdaExpression *)malloc(sizeof(struct LambdaExpression));
  res_2->node.lambda_expr->parameter = 'c';

  struct AstNode *body = (struct AstNode *)malloc(sizeof(struct AstNode));
  body->type = VAR;
  body->node.variable = (struct Variable *)malloc(sizeof(struct Variable));
  body->node.variable->name = 'd';
  res_2->node.lambda_expr->body = body;

  print_ast(res_2);
  return 0;
}


void print_ast(struct AstNode *node) {
  if(node == NULL) {
    return;
  }

  switch(node->type) {
    case LAMBDA_EXPR:
        printf("(LAMBDA %c ", node->node.lambda_expr->parameter);
        print_ast(node->node.lambda_expr->body);
        printf(") ");
        break;

    case APPLICATION:
        printf("(APP ");
        print_ast(node->node.application->function);
        print_ast(node->node.application->argument);
        printf(") ");
        break;

    case VAR:
        printf("(VAR %c) ", node->node.variable->name);
        break;

    default:
        printf("(UNKNOWN) ");
  }
}
// grammar
// Expression ::= LambdaExpression | Application | Variable
// LambdaExpression ::= "@" Identifier "." Expression
// Application ::= "(" Expression Expression ")"
// Variable ::= Identifier

