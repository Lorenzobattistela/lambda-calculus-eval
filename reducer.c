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
    n->node.lambda_expr->body = reduce_ast(n->node.lambda_expr->body);
    return n;
  }

  else if(n->type == APPLICATION) {
    // apply reduce rules

    n->node.application->function = reduce_ast(n->node.application->function);
    n->node.application->argument = reduce_ast(n->node.application->argument);

    if(n->node.application->function->type == LAMBDA_EXPR) {
      return substitute(
        n->node.application->function->node.lambda_expr->body, 
        n->node.application->function->node.lambda_expr->parameter, 
        n->node.application->argument
      );
    }
    return n;
  }
  // base case leaf node or variable, nothing to reduce
  return n;
}

struct AstNode *substitute(struct AstNode *expression, char variable, struct AstNode *replacement) {
  if(expression->type == VAR) {
    if (expression->node.variable->name == variable) {
      return deepcopy(replacement);
    }
    return expression;
  }

  else if(expression->type == LAMBDA_EXPR) {
    if(expression->node.lambda_expr->parameter == variable) {
      return expression;
    } 
    expression->node.lambda_expr->body = substitute(
      expression->node.lambda_expr->body, variable, replacement
    );
    return expression;
  
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

struct AstNode *deepcopy(struct AstNode *n) {
  if(n->type == VAR) {
    return deepcopy_var(n->node.variable->name);
  }

  else if(n->type == LAMBDA_EXPR) {
    return deepcopy_lambda_expr(
      n->node.lambda_expr->parameter, 
      n->node.lambda_expr->body
    );
  }

  else if(n->type == APPLICATION) {
    return deepcopy_application(
      n->node.application->function,
      n->node.application->argument
    );
  }
  return NULL;
}

struct AstNode *deepcopy_application(struct AstNode *function, struct AstNode *argument) {
  struct AstNode *application = (struct AstNode *)malloc(sizeof(struct AstNode));
  application->type = APPLICATION;
  application->node.application = (struct Application *)malloc(sizeof(struct Application));
  application->node.application->function = deepcopy(function);
  application->node.application->argument = deepcopy(argument);
  return application;
}

struct AstNode *deepcopy_lambda_expr(char parameter, struct AstNode *body) {
  struct AstNode *lambda = (struct AstNode *)malloc(sizeof(struct AstNode));
  lambda->type = LAMBDA_EXPR;
  lambda->node.lambda_expr = (struct LambdaExpression *)malloc(sizeof(struct LambdaExpression));
  lambda->node.lambda_expr->parameter = parameter;
  lambda->node.lambda_expr->body = deepcopy(body);
  return lambda;
}

struct AstNode *deepcopy_var(char name) {
  struct AstNode *variable = (struct AstNode *)malloc(sizeof(struct AstNode));
  variable->type = VAR;
  variable->node.variable = (struct Variable *)malloc(sizeof(struct Variable));
  variable->node.variable->name = name;
  return variable;
}
