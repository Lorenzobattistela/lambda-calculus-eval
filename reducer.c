#include "reducer.h"

// Alpha conversion -> if we have two lambda expressions with same variable name, change one of
// them to create a new variable: (@x.xx)(@x.x) -> (@x.xx)(@y.y) or -> (@x.xx)(@x'.x')

// Beta reduction: substitution -> we have to look for scope (@x.xy)z => zy

// Eta Conversion / reduction -> Converts between @x.(f x) and f whenever x does not appear free in f 
// which means @x.(f x) = f if f does not make use of x
// @x.(@y.yy)x) is equivalent to (@y.yy) because f does not make use of x.


struct AstNode *reduce_ast(struct AstNode *n) {
  if(n->type == LAMBDA_EXPR) {
    // apply reduction rules

    // recursively reduce the body
    n->node.lamba_expr->body = reduce_ast(n->node.lambda_expr->body);
    return n;
  }

  else if(n->type == APPLICATION) {
    // apply reduce rules

    n->node.application->function = reduce_ast(n->node.application->function);
    n->node.application->argument = reduce_ast(n->node.application->argument);

    if(n->node.application->function->node.type == LAMBDA_EXPR) {
      return substitute(
        n->node.application->function->node.lambda_expr->body, 
        n->node.application->function->node.lamba_expr->parameter, 
        n->node.lambda_expr->argument
      );
    }
    return n;
  }
  return n;
}

struct AstNode *substitute(struct AstNode *expression, char variable, struct AstNode *replacement) {
  if(expression->type == VAR) {
    if (expression->node.variable->name == variable) {
      return deepcopy(replacement);
    }
    else return expression;
  }

  else if(expression->type == LAMBDA_EXPR) {
    if(expression->node.lamba_expr->parameter == variable) {
      return expression;
    }
    else {
      expression->node.lambda_expr->body = substitute(
        expression->node.lamba_expr->body, variable, replacement
      );
      return expression;
    }
  }

  else if(expression->type == APPLICATION) {
    expression->node.application->function = substitute(
      expression->node.application->function, variable, replacement
    );
    expression->node.application->argument = substitute(
      expression->node.application->argument, variable, replacement
    );
    return expression;
  }
  return expression;
}
