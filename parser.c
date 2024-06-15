#include "parser.h"

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
  }
}

void print_ast(AstNode *node) {
  if (node == NULL) {
    return;
  }

  switch (node->type) {
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

bool is_variable(char token) {
  int cmp = (int)token;
  if (cmp < 97 || cmp > 122) {
    return false;
  }
  return true;
}

AstNode *parse_expression(FILE *in, char token) {
  while (parse_token(token) == WHITESPACE) {
    token = next(in);
  }
  tokens_t scanned = parse_token(token);

  if (scanned == LAMBDA) {
    char parameter = next(in);
    tokens_t param = parse_token(parameter);
    if (param != VARIABLE) {
      expect("a variable from a-z", token);
      return NULL;
    }

    char dot = next(in);
    tokens_t dot_t = parse_token(dot);
    if (dot_t != DOT) {
      expect(".", token);
      return NULL;
    }

    AstNode *body = parse_expression(in, next(in));

    AstNode *res = (AstNode *)malloc(sizeof(AstNode));
    res->type = LAMBDA_EXPR;
    res->node.lambda_expr =
        (LambdaExpression *)malloc(sizeof(LambdaExpression));
    res->node.lambda_expr->parameter = parameter;
    res->node.lambda_expr->body = body;
    return res;
  }

  else if (scanned == L_PAREN) {
    AstNode *expr = parse_expression(in, next(in));
    char next_t = next(in);
    // if it is a whitespace, it is a function application
    tokens_t next_token = parse_token(next_t);
    if (next_token == WHITESPACE) {
      AstNode *expr_2 = parse_expression(in, next(in));
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
    AstNode *variable = (AstNode *)malloc(sizeof(AstNode));
    variable->type = VAR;
    variable->node.variable = (Variable *)malloc(sizeof(Variable));
    variable->node.variable->name = token;
    return variable;
  }
  return NULL;
}

void expect(char *expected, char received) {
  printf("Syntax Error: Expected %s , received %c \n", expected, received);
  exit(1);
}

// Expression ::= LambdaExpression | Application | Variable
// LambdaExpression ::= "@" Identifier "." Expression
// Application ::= "(" Expression Expression ")"
// Variable ::= Identifier
