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

void append_to_buffer(char **buffer, size_t *buffer_size, size_t *length, const char *str) {
    size_t str_len = strlen(str);
    while (*length + str_len + 1 >= *buffer_size) {
        *buffer_size *= 2;
        *buffer = realloc(*buffer, *buffer_size);
    }
    strcpy(*buffer + *length, str);
    *length += str_len;
}

void append_ast_to_buffer(char **buffer, size_t *buffer_size, size_t *length, AstNode *node) {
    if (node == NULL) {
        return;
    }

    switch (node->type) {
        case LAMBDA_EXPR:
            append_to_buffer(buffer, buffer_size, length, "(@");
            append_to_buffer(buffer, buffer_size, length, node->node.lambda_expr->parameter);
            append_to_buffer(buffer, buffer_size, length, ".");
            append_ast_to_buffer(buffer, buffer_size, length, node->node.lambda_expr->body);
            append_to_buffer(buffer, buffer_size, length, ") ");
            break;

        case APPLICATION:
            append_to_buffer(buffer, buffer_size, length, "(");
            append_ast_to_buffer(buffer, buffer_size, length, node->node.application->function);
            append_ast_to_buffer(buffer, buffer_size, length, node->node.application->argument);
            append_to_buffer(buffer, buffer_size, length, ") ");
            break;

        case VAR:
            append_to_buffer(buffer, buffer_size, length, "(");
            append_to_buffer(buffer, buffer_size, length, node->node.variable->name);
            append_to_buffer(buffer, buffer_size, length, ") ");
            break;

        case DEFINITION:
            append_to_buffer(buffer, buffer_size, length, "(");
            append_to_buffer(buffer, buffer_size, length, node->node.variable->name);
            append_to_buffer(buffer, buffer_size, length, ") ");
            break;

        default:
            append_to_buffer(buffer, buffer_size, length, "(UNKNOWN) ");
    }
}

char *ast_to_string(AstNode *node) {
    size_t buffer_size = 1024;
    char *buffer = malloc(buffer_size);
    buffer[0] = '\0';
    size_t length = 0;
    append_ast_to_buffer(&buffer, &buffer_size, &length, node);
    return buffer;
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
  printf("VAR IS: %s\n", var);
  char *new_var = NULL;
  if (is_used(table, var)) {
    new_var = alpha_convert(var);
    insert(table, new_var, NULL);
  } else {
    insert(table, var, NULL);
  }
  char dot = next(in);
  if (parse_token(dot) != DOT) {
    expect(".", dot);
  }
  AstNode *body = parse_expression(table, in, next(in));
  if (new_var != NULL) {
    replace(body, var, new_var);
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

      char right_paren = next(in);
      if (parse_token(right_paren) != R_PAREN) {
        expect(")", right_paren);
      }
      return application;
    }

    if (next_token != R_PAREN) {
      expect(")", next_token);
      return NULL;
    }
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
    }

    AstNode *variable = create_variable(var_name);
    if (search(table, var_name) != NULL) {
      variable->type = DEFINITION;
    }
    return variable;
  }
  return NULL;
}

void parse_definition(HashTable *table, FILE *in) {
  char next_token = next(in);
  tokens_t n = parse_token(next_token);
  if (n != WHITESPACE) {
    expect(" ", next_token);
    exit(EXIT_FAILURE);
  }

  next_token = next(in);
  n = parse_token(next_token);

  if (n != VARIABLE) {
    expect("a variable", next_token);
    exit(EXIT_FAILURE);
  }
  char *def_name = parse_variable(in, next_token);

  next_token = next(in);
  n = parse_token(next_token);
  if (n != WHITESPACE) {
    expect(" ", next_token);
    exit(EXIT_FAILURE);
  }

  next_token = next(in);
  n = parse_token(next_token);
  if (n != EQ) {
    expect("=", next_token);
    exit(EXIT_FAILURE);
  }

  next_token = next(in);
  n = parse_token(next_token);
  if (n != WHITESPACE) {
    expect(" ", next_token);
    exit(EXIT_FAILURE);
  }

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
