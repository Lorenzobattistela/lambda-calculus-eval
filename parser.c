#include "parser.h"

tokens_t parse_token(char *token) {
  if(strcmp(token, "(") == 0) {
    return L_PAREN;
  }
  else if(strcmp(token, ")") == 0) {
    return R_PAREN;
  }
  else if(strcmp(token, "@") == 0) {
    return LAMBDA;
  }
  else if(strcmp(token, ".") == 0) {
    return DOT;
  }
  else if(is_variable(token)) {
    return VARIABLE;
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

    default:
      printf("ERROR ");
      return;
  }
}

bool is_variable(char *token) {
  if(!isalpha(token[0]) || islower(token[0])) {
    return false;
  }

  for(int i = 1; i < strlen(token); i++) {
    if(!isalpha(token[i])) {
      return false;
    }
  }
  return true;
}

// now we build structs for each "type" / kind in a lambda exp and build a parser that consumes the tokens produced by the scanner
// and that puts them into an AST 
struct LambdaExpression {
  char parameter;
  struct AstNode *body;
};

struct Application {
  struct AstNode *function;
  struct AstNode *argument;
};

struct Variable {
  char name;
};

AstNode parse_expression(tokens_t token) {
  if(token == LAMBDA) {
    return parse_lambda_expression();
  }
  else if(token == LPAREN) {
    return parse_application();
  }
  else if(token == VARIABLE) {
    return parse_variable();
  }
}

AstNode parse_lambda_expression(FILE *in) {
  // get next token
  
}


// Expression ::= LambdaExpression | Application | Variable
// LambdaExpression ::= "@" Identifier "." Expression
// Application ::= "(" Expression Expression ")"
// Variable ::= Identifier

