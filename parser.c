#include "parser.h"

tokens_t parse_token(char token) {
  if(token == '(') {
    return L_PAREN;
  }
  else if(token == ')') {
    return R_PAREN;
  }
  else if(token == '@') {
    return LAMBDA;
  }
  else if(token == '.') {
    return DOT;
  }
  else if(is_variable(token)) {
    return VARIABLE;
  }
  else if(token == ' ') {
    return WHITESPACE;
  }
  return ERROR;
}

void p_print_token(tokens_t token) {
  switch(token) {
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

bool is_variable(char token) {
  int cmp = (int)token;
  if(cmp < 97 || cmp > 122) {
    return false;
  }
  return true;
}

struct AstNode *parse_expression(FILE *in, char token) {
  while(parse_token(token) == WHITESPACE) {
    token = next(in);
  }

  tokens_t scanned = parse_token(token);
  
  if(scanned == LAMBDA) {
    char parameter = next(in);
    tokens_t param = parse_token(parameter);
    if(param != VARIABLE) {
      expect("a variable from a-z", token);
      return NULL;
    }

    char dot = next(in);
    printf("%c\n", dot);
    tokens_t dot_t = parse_token(dot);
    if(dot_t != DOT) {
      expect(".", token);
      return NULL;
    }

    struct AstNode *body = parse_expression(in, next(in));

    struct AstNode *res = (struct AstNode *)malloc(sizeof(struct AstNode));
    res->type = LAMBDA_EXPR;
    res->node.lambda_expr = (struct LambdaExpression *)malloc(sizeof(struct LambdaExpression));
    res->node.lambda_expr->parameter = parameter;
    res->node.lambda_expr->body = body;
    return res;
  }

  else if(scanned == L_PAREN) {
    struct AstNode *expr = parse_expression(in, next(in));
    char r_paren = next(in);
    tokens_t r_paren_t = parse_token(r_paren);
    if(r_paren_t != R_PAREN) {
      expect(")", r_paren);
      return NULL;
    }
    return expr;
  }

  else if(scanned == VARIABLE) {
    struct AstNode *variable = (struct AstNode *)malloc(sizeof(struct AstNode));
    variable->type = VAR;
    variable->node.variable = (struct Variable *)malloc(sizeof(struct Variable));
    variable->node.variable->name = token;
    return variable;
  }
  return NULL;
}

void expect(char *expected, char received) {
  printf("ERROR: Expected %s , received %c \n", expected, received);
}

// Expression ::= LambdaExpression | Application | Variable
// LambdaExpression ::= "@" Identifier "." Expression
// Application ::= "(" Expression Expression ")"
// Variable ::= Identifier

