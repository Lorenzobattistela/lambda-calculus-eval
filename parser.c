#include "parser.h"
#include "common.h"
#include "hash-table/hash_table.h"
#include "io.h"

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

AstNode *parse_expression(HashTable *table, FILE *in, char token) {
  while (parse_token(token) == WHITESPACE || parse_token(token) == NEWLINE) {
    token = next(in);
  }
  tokens_t scanned = parse_token(token);

  if (scanned == LAMBDA) {
    char parameter = next(in);
    tokens_t param = parse_token(parameter);
    if (param != VARIABLE) {
      return NULL;
    }

    char *var = parse_variable(in, parameter);
    char dot = next(in);
    tokens_t dot_t = parse_token(dot);
    if (dot_t != DOT) {
      expect(".", token);
      return NULL;
    }

    AstNode *body = parse_expression(table, in, next(in));

    AstNode *res = (AstNode *)malloc(sizeof(AstNode));
    res->type = LAMBDA_EXPR;
    res->node.lambda_expr =
        (LambdaExpression *)malloc(sizeof(LambdaExpression));
    res->node.lambda_expr->parameter = var;
    res->node.lambda_expr->body = body;
    return res;
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
    AstNode *variable = (AstNode *)malloc(sizeof(AstNode));

    HANDLE_NULL(variable);
    variable->type = VAR;
    if (search(table, var_name) != NULL) {
      variable->type = DEFINITION;
    }
    variable->node.variable = (Variable *)malloc(sizeof(Variable));
    variable->node.variable->name = var_name;
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
  printf("def name is: %s\n", def_name);

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
  exit(1);
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
