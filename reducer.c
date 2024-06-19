#include "reducer.h"
#include "common.h"
#include "hash-table/hash_table.h"

#define VERBOSE 0

#if VERBOSE
  #define SHOW(a) printf a
#else
  #define SHOW(a) (void)0
#endif

bool used_variables[SIZE] = {false};

// Alpha conversion -> if we have two lambda expressions with same variable
// name, change one of them to create a new variable: (@x.xx)(@x.x) ->
// (@x.xx)(@y.y) or -> (@x.xx)(@x'.x')

// Beta reduction: substitution -> we have to look for scope (@x.xy)z => zy

// Eta Conversion / reduction -> Converts between @x.(f x) and f whenever x does
// not appear free in f which means @x.(f x) = f if f does not make use of x
// @x.(@y.yy)x) is equivalent to (@y.yy) because f does not make use of x.

void set_variable(char variable) { used_variables[(int)variable] = true; }
void set_verbose(bool verbose) {
  #undef VERBOSE
  #define VERBOSE verbose
  printf("Verbose set to: %i\n", verbose);
}

char new_variable() {
  for (int i = 0; i < SIZE; i++) {
    if (!used_variables[i]) {
      return (char)i;
    }
  }
  printf("No more available variables to do alpha conversion. Quitting");
  exit(1);
}

bool is_used(char *variable) { return false; }

void expand_definitions(HashTable *table, AstNode *n) {
  if (n->type == LAMBDA_EXPR) {
    expand_definitions(table, n->node.lambda_expr->body);
  } else if (n->type == APPLICATION) {
    expand_definitions(table, n->node.application->function);
    expand_definitions(table, n->node.application->argument);
  } else if (n->type == DEFINITION) {
    char *def_name = n->node.variable->name;
    AstNode *expanded_def = search(table, def_name);
    HANDLE_NULL(expanded_def);
    n->type = expanded_def->type;
    n->node.variable = expanded_def->node.variable;
    n->node.lambda_expr = expanded_def->node.lambda_expr;
    n->node.application = expanded_def->node.application;
  }
}

void replace(AstNode *n, char *old, char *new_name) {
  if (n->type == LAMBDA_EXPR) {
    if (strcmp(n->node.lambda_expr->parameter, old) == 0) {
      n->node.lambda_expr->parameter = new_name;
    }
    replace(n->node.lambda_expr->body, old, new_name);
  }

  else if (n->type == APPLICATION) {
    replace(n->node.application->function, old, new_name);
    replace(n->node.application->argument, old, new_name);
  }

  else if (n->type == VAR) {
    if (strcmp(n->node.variable->name, old)) {
      n->node.variable->name = new_name;
    }
  }
}

AstNode *reduce_ast(AstNode *n) {
  if (n->type == LAMBDA_EXPR) {
    // if a variable is already used and we encounter it on a lambda_expr, we
    // should rename it and replace it across the body of the lamba expr

    if (is_used(n->node.lambda_expr->parameter)) {
      SHOW(("Performing alpha conversion on param: %s\n", n->node.lambda_expr->paramater));
      printf("TODO\n");
      exit(EXIT_FAILURE);
      // char new = new_variable();
      // replace(n, n->node.lambda_expr->parameter, new);
      // set_variable(new);
    }

    // recursively reduce the body
    SHOW(("Reducing lambda %s expression body\n", n->node.lambda_expr->parameter));
    n->node.lambda_expr->body = reduce_ast(n->node.lambda_expr->body);
    return n;
  }

  else if (n->type == APPLICATION) {
    // apply reduce rules
    SHOW(("Reducing application function\n"));
    n->node.application->function = reduce_ast(n->node.application->function);
    SHOW(("Reducing application argument\n"));
    n->node.application->argument = reduce_ast(n->node.application->argument);

    if (n->node.application->function->type == LAMBDA_EXPR) {
      SHOW(("Performing beta reduction\n"));
      return substitute(
          n->node.application->function->node.lambda_expr->body,
          n->node.application->function->node.lambda_expr->parameter,
          n->node.application->argument);
    }
    return n;
  }

  // base case leaf node or variable, nothing to reduce
  return n;
}

AstNode *substitute(AstNode *expression, char *variable, AstNode *replacement) {
  if (expression->type == VAR) {
    if (strcmp(expression->node.variable->name, variable) == 0) {
      return deepcopy(replacement);
    }
    return expression;
  }

  else if (expression->type == LAMBDA_EXPR) {
    // ex: @x.(x y) -> if variable is equal to @x (x), we should then replace
    // all x in the body of expression with replacement
    expression->node.lambda_expr->body =
        substitute(expression->node.lambda_expr->body, variable, replacement);
    if (expression->node.lambda_expr->parameter != variable) {
      // if @x.(x y) and we're substituting for y, for example, we do not want
      // to remove the lambda term
      return expression;
    }
    return expression->node.lambda_expr->body;

  }

  else if (expression->type == APPLICATION) {
    expression->node.application->function = substitute(
        expression->node.application->function, variable, replacement);
    expression->node.application->argument = substitute(
        expression->node.application->argument, variable, replacement);
    return expression;
  }
  return expression;
}

AstNode *deepcopy(AstNode *n) {
  if (n->type == VAR) {
    return deepcopy_var(n->node.variable->name);
  }

  else if (n->type == LAMBDA_EXPR) {
    return deepcopy_lambda_expr(n->node.lambda_expr->parameter,
                                n->node.lambda_expr->body);
  }

  else if (n->type == APPLICATION) {
    return deepcopy_application(n->node.application->function,
                                n->node.application->argument);
  }
  return NULL;
}

AstNode *deepcopy_application(AstNode *function, AstNode *argument) {
  AstNode *application = (AstNode *)malloc(sizeof(AstNode));
  application->type = APPLICATION;
  application->node.application = (Application *)malloc(sizeof(Application));
  application->node.application->function = deepcopy(function);
  application->node.application->argument = deepcopy(argument);
  return application;
}

AstNode *deepcopy_lambda_expr(char *parameter, AstNode *body) {
  AstNode *lambda = (AstNode *)malloc(sizeof(AstNode));
  lambda->type = LAMBDA_EXPR;
  lambda->node.lambda_expr =
      (LambdaExpression *)malloc(sizeof(LambdaExpression));
  lambda->node.lambda_expr->parameter = parameter;
  lambda->node.lambda_expr->body = deepcopy(body);
  return lambda;
}

AstNode *deepcopy_var(char *name) {
  AstNode *variable = (AstNode *)malloc(sizeof(AstNode));
  variable->type = VAR;
  variable->node.variable = (Variable *)malloc(sizeof(Variable));
  variable->node.variable->name = name;
  return variable;
}
