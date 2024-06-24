#include "parser.h"
#include "common.h"
#include "hash-table/hash_table.h"
#include "io.h"
#include "reducer.h"
#include <string.h>
#include <math.h>

static int n = 1;

tokens_t parse_token(char token) {
  if (token == '(') {
    return L_PAREN;
  } else if (token == ')') {
    return R_PAREN;
  } else if (token == '@') {
    return LAMBDA;
  } else if (token == '.') {
    return DOT;
  } else if (is_variable(token)) {
    return VARIABLE;
  } else if (token == ' ') {
    return WHITESPACE;
  } else if (token == '\n') {
    return NEWLINE;
  } else if (token == '=') {
    return EQ;
  } else if (token == '"') {
    return QUOTE;
  }
  return ERROR;
}

void p_print_token(tokens_t token) {
  switch (token) {
  case L_PAREN:
    printf("( ");
    return;

  case R_PAREN:
    printf(") ");
    return;

  case LAMBDA:
    printf("@ ");
    return;

  case DOT:
    printf(". ");
    return;

  case VARIABLE:
    printf("VARIABLE ");
    return;

  case WHITESPACE:
    printf("WHITESPACE ");
    return;

  case NEWLINE:
    printf("NEWLINE ");
    return;

  case EQ:
    printf("= ");
    return;

  default:
    printf("ERROR ");
    return;
  }
}

void p_print_astNode_type(AstNode *n) {
  switch (n->type) {
  case 0:
    printf("AstNode Type: LAMBDA_EXPR\n");
    break;

  case 1:
    printf("AstNode Type: APPLICATION\n");
    break;

  case 2:
    printf("AstNode Type: VAR\n");
    break;

  case DEFINITION:
    printf("AstNode Type: DEFINITION\n");
    break;
  }
}

void print_ast(AstNode *node) {
  if (node == NULL) {
    return;
  }

  switch (node->type) {
  case LAMBDA_EXPR:
    printf("(LAMBDA %s ", node->node.lambda_expr->parameter);
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
    printf("(VAR %s) ", node->node.variable->name);
    break;

  case DEFINITION:
    printf("(DEFINITION %s) ", node->node.variable->name);
    break;

  default:
    printf("(UNKNOWN) ");
  }
}

bool is_variable(char token) {
  int cmp = (int)token;
  if (cmp == '_') return true;
  if (cmp < 97 || cmp > 122) {
    return false;
  }
  return true;
}

char peek(FILE *in) {
  char c = fgetc(in);
  ungetc(c, in);
  return c;
}

void consume(tokens_t t, FILE *in, char *expected) {
  char c = next(in);
  tokens_t p = parse_token(c);
  if (p != t) {
    expect(expected, c);
  }
}

AstNode *create_variable(char *name) {
  AstNode *var = (AstNode *)malloc(sizeof(AstNode));
  HANDLE_NULL(var);
  var->type = VAR;
  var->node.variable = (Variable *)malloc(sizeof(Variable));
  var->node.variable->name = name;
  return var;
}

AstNode *create_application(AstNode *function, AstNode *argument) {
  AstNode *app = (AstNode *)malloc(sizeof(AstNode));
  HANDLE_NULL(app);
  app->type = APPLICATION;
  app->node.application = (Application *)malloc(sizeof(Application));
  app->node.application->function = function;
  app->node.application->argument = argument;
  return app;
}

AstNode *create_lambda(char *variable, AstNode *body) {
  AstNode *lambda = (AstNode *)malloc(sizeof(AstNode));
  HANDLE_NULL(lambda);
  lambda->type = LAMBDA_EXPR;
  lambda->node.lambda_expr =
      (LambdaExpression *)malloc(sizeof(LambdaExpression));
  lambda->node.lambda_expr->parameter = variable;
  lambda->node.lambda_expr->body = body;
  return lambda;
}



// Alpha conversion -> if we have two lambda expressions with same variable
// name, change one of them to create a new variable: (@x.xx)(@x.x) ->
// (@x.xx)(@y.y) or -> (@x.xx)(@x'.x')

// Beta reduction: substitution -> we have to look for scope (@x.xy)z => zy

// Eta Conversion / reduction -> Converts between @x.(f x) and f whenever x does
// not appear free in f which means @x.(f x) = f if f does not make use of x
// @x.(@y.yy)x) is equivalent to (@y.yy) because f does not make use of x.

// Just convert var to var_n
char *alpha_convert(char *old) {
  int digits = (int)log10(abs(n)) + 1;

  char *new = malloc(strlen(old) + digits + 2);
  HANDLE_NULL(new);
  char num_str[digits + 1];

  strcpy(new, old);

  sprintf(num_str, "_%d", n);
  strcat(new, num_str);

  n++;

  return new;
}

bool is_used(HashTable *table, char *variable) {
  return table_exists(table, variable);
}


AstNode *parse_lambda(HashTable *table, FILE *in) {
  char parameter = next(in);
  tokens_t param = parse_token(parameter);
  if (param != VARIABLE) {
    expect("A variable", parameter);
  }

  char *var = parse_variable(in, parameter);
  char *new_var = NULL;
  if (is_used(table, var)) {
    if (search(table, var) != NULL) {
      const char * error_msg = format("A definition with name %s already exists. Cannot use same name for lambda abstraction.\n", var);
      error(error_msg, __FILE__, __LINE__, __func__);
    }
    new_var = alpha_convert(var);
    insert(table, new_var, NULL);
  } else {
    insert(table, var, NULL);
  }

  consume(DOT, in, ".");

  AstNode *body = parse_expression(table, in, next(in));
  if (new_var != NULL) {
    replace(body, var, new_var);
    print_verbose("Alpha converted %s to %s\n", var, new_var);
    return create_lambda(new_var, body);
  }
  return create_lambda(var, body);
}

AstNode *parse_expression(HashTable *table, FILE *in, char token) {
  while (parse_token(token) == WHITESPACE || parse_token(token) == NEWLINE) {
    token = next(in);
  }
  tokens_t scanned = parse_token(token);

  if (scanned == LAMBDA) {
    return parse_lambda(table, in);
  }

  else if (scanned == L_PAREN) {
    AstNode *expr = parse_expression(table, in, next(in));
    char next_t = next(in);
    // if it is a whitespace, it is a function application
    tokens_t next_token = parse_token(next_t);
    if (next_token == WHITESPACE) {
      AstNode *expr_2 = parse_expression(table, in, next(in));
      AstNode *application = (AstNode *)malloc(sizeof(AstNode));
      application->type = APPLICATION;
      application->node.application =
          (Application *)malloc(sizeof(Application));
      application->node.application->function = expr;
      application->node.application->argument = expr_2;

      consume(R_PAREN, in, ")");

      return application;
    }

    consume(R_PAREN, in, ")");

    return expr;
  }

  else if (scanned == VARIABLE) {
    char *var_name = parse_variable(in, token);

    if (strcmp(var_name, "def") == 0) {
      parse_definition(table, in);
      if (peek(in) != EOF) {
        return parse_expression(table, in, next(in));
      }
      return NULL;
    } else if (strcmp(var_name, "import") == 0) {
      parse_import(table, in);
      if (peek(in) != EOF) {
        return parse_expression(table, in, next(in));
      }
    }

    AstNode *variable = create_variable(var_name);
    if (search(table, var_name) != NULL) {
      variable->type = DEFINITION;
    }
    return variable;
  }
  return NULL;
}

void parse_import(HashTable *table, FILE *in) {
    // in this step we already parsed the string "import"
    char next_token;
    tokens_t n;

    consume(WHITESPACE, in, "a whitespace");

    consume(QUOTE, in, "\"");

    char *file_path = malloc(100 * sizeof(char));
    HANDLE_NULL(file_path);

    next_token = next(in);
    n = parse_token(next_token);

    file_path[0] = '\0';
    int current_length = 0;

    while (n != QUOTE) {
      // parse the file path
      if (current_length < 100 - 1) {
        file_path[current_length] = next_token;
        current_length++;
        file_path[current_length] = '\0';
      } else {
        const char *error_msg = "File path is too long. Please make sure it is less than 100 characters.";
        error(error_msg, __FILE__, __LINE__, __func__);
      }

      next_token = next(in);
      n = parse_token(next_token);
    }

    if (n != QUOTE) {
      expect("a closing quote", next_token);
    }

    FILE *imported_file = get_file(file_path, "r");
    HANDLE_NULL(imported_file);

    char imported_tkn;
    while ((imported_tkn = next(imported_file)) != EOF) {
      tokens_t scanned = parse_token(imported_tkn);
      while (scanned == WHITESPACE || scanned == NEWLINE) {
        imported_tkn = next(imported_file);
        scanned = parse_token(imported_tkn);
      }
      if (scanned == VARIABLE) {
        char *var_name = parse_variable(imported_file, imported_tkn);
        if (strcmp(var_name, "def") == 0) {
          parse_definition(table, imported_file);
        } else {
          const char *error_msg = format("Expected a definition in the imported file, but got %s\n", var_name);
          error(error_msg, __FILE__, __LINE__, __func__);
        }
      }
    }
    return;
}

void parse_definition(HashTable *table, FILE *in) {
  // at this moment the "def" string was already parsed
  char next_token;
  tokens_t n;

  consume(WHITESPACE, in, "a whitespace");

  next_token = next(in);
  n = parse_token(next_token);
  if (n != VARIABLE) {
    expect("a variable", next_token);
  }

  char *def_name = parse_variable(in, next_token);

  consume(WHITESPACE, in, "a whitespace");

  consume(EQ, in, "=");

  consume(WHITESPACE, in, "a whitespace");

  AstNode *definition = parse_expression(table, in, next(in));
  insert(table, def_name, definition);
}

char *parse_variable(FILE *in, tokens_t token) {
  char *variable_name = malloc(256 * sizeof(char));
  int index = 0;
  variable_name[index] = token;
  index++;

  while (is_variable(peek(in))) {
    variable_name[index] = next(in);
    index++;
  }
  return variable_name;
}

void expect(char *expected, char received) {
  printf("Syntax Error: Expected %s , received %c \n", expected, received);
  exit(EXIT_FAILURE);
}

void free_ast(AstNode *node) {
  if (node == NULL) {
    return;
  }

  switch (node->type) {
  case LAMBDA_EXPR:
    if (node->node.lambda_expr != NULL) {
      free(node->node.lambda_expr->parameter);
      free_ast(node->node.lambda_expr->body);
      free(node->node.lambda_expr);
    }
    break;

  case APPLICATION:
    if (node->node.application != NULL) {
      free_ast(node->node.application->function);
      free_ast(node->node.application->argument);
      free(node->node.application);
    }
    break;

  case VAR:
    if (node->node.variable != NULL) {
      free(node->node.variable->name);
      free(node->node.variable);
    }
    break;

  default:
    break;
  }

  free(node);
}

// Expression ::= LambdaExpression | Application | Variable
// LambdaExpression ::= "@" Identifier "." Expression
// Application ::= "(" Expression Expression ")"
// Variable ::= Identifier
