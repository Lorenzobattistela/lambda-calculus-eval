#include "io.h"
#include "parser.h"
#include "reducer.h"
#include <stdio.h>

void print_ast(struct AstNode *node);

int main(void) {
  FILE *in = get_file("expr.lambda", "r");
  struct AstNode *res = parse_expression(in, next(in)); 
  printf("Parsing expresion on file expr.lambda: \n");
  print_ast(res);
  printf("\n\nTesting deepcopy on custom expression @c.(c): \n");
  struct AstNode *res_2 = (struct AstNode *)malloc(sizeof(struct AstNode));
  res_2->type = LAMBDA_EXPR;
  res_2->node.lambda_expr = (struct LambdaExpression *)malloc(sizeof(struct LambdaExpression));
  res_2->node.lambda_expr->parameter = 'c';

  struct AstNode *body = (struct AstNode *)malloc(sizeof(struct AstNode));
  body->type = VAR;
  body->node.variable = (struct Variable *)malloc(sizeof(struct Variable));
  body->node.variable->name = 'c';
  res_2->node.lambda_expr->body = body;
  print_ast(deepcopy(res_2));

  printf("\n\nTesting substitution: @c.(c) @x.x \n");
  struct AstNode *substitution = substitute(res_2, 'c', res);
  print_ast(substitution);

  FILE *in_2 = get_file("expr.lambda2", "r");
  struct AstNode *app = parse_expression(in_2, next(in_2));
  printf("\n\nParsing expression on expr.lambda2: \n");
  print_ast(app);
  printf("\n\nTesting substitution on it: \n");
  struct AstNode *func_app = substitute(app, 'x', res);
  print_ast(func_app);
  
  printf("\n\nReplacing all x for g on the previous result: ");
  replace(func_app, 'x', 'g');
  printf("\n");
  print_ast(func_app);

  printf("\n\n Testing AST reduction:\n");
  FILE *third = get_file("expr.lambda3", "r");
  struct AstNode *ast = parse_expression(third, next(third));
  printf("Parsed AST for third expr: \n");
  print_ast(ast);
  printf("\n\nReduced ast:\n");
  struct AstNode *reduced = reduce_ast(ast);
  print_ast(reduced);

  close_file(in);
  close_file(in_2);
  close_file(third);

  // changeable expr:
  FILE *fourth = get_file("expr.lambda4", "r");
  printf("\nReading fourth expression: \n");
  struct AstNode *four = parse_expression(fourth, next(fourth));
  print_ast(four);
  printf("\n\nReducing fourth expression: \n");
  struct AstNode *fourth_reduced = reduce_ast(four);
  print_ast(fourth_reduced);
  printf("\n");
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

