#include "io.h"
#include "parser.h"
#include <stdio.h>

void print_ast(struct AstNode *node);

int main(void) {
  FILE *in = get_file("expr.lambda", "r");
  
  struct AstNode *res = parse_expression(in, next(in)); 
  print_ast(res);
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

