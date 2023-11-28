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

AstNode parse_expression(FILE *in, char token) {
  tokens_t scanned = parse_token(token);
  if(scanned == LAMBDA) {
    char parameter = next(in);
    tokens_t param = parse_token(token);
    if(param != VARIABLE) {
      expect("a variable from a-z", token);
      return NULL;
    }

    char dot = next(in);
    tokens_t dot_t = parse_token(dot);
    if(dot_t != DOT) {
      expect(".", token);
      return;
    }

    AstNode body = parse_expression(in, next(in));
  }

  else if(scanned == L_PAREN) {
    AstNode expr = parse_expression(in, next(in));
    // how would ik if the token is the correct one
    // cause when i reach this, it means the next token is ')', 
    // but how to expect this token?
    // the call to parse_exp would go N tokens before returning a AstNode
    // but i'll try getting the next one
    char r_paren = next(in);
    tokens_t r_paren_t = parse_token(r_paren);
    if(r_paren_t != R_PARAM) {
      expect(")", r_paren);
      return;
    }
    return expr;
  }
}

void expect(char *expected, char received) {
  printf("ERROR: Expected %c , received %c \n", expected, received);
}

// now we build structs for each "type" / kind in a lambda exp and build a parser that consumes the tokens produced by the scanner
// and that puts them into an AST 

// AstNode parse_expression(tokens_t token) {
  // if(token == LAMBDA) {
    // return parse_lambda_expression();
  // } 
  // else if(token == LPAREN) {
    // return parse_application();
  // }
  // else if(token == VARIABLE) {
    // return parse_variable();
  // }
 //}

// struct AstNode parse_lambda_expression(FILE *in) {
  // get next token
  
//}


// Expression ::= LambdaExpression | Application | Variable
// LambdaExpression ::= "@" Identifier "." Expression
// Application ::= "(" Expression Expression ")"
// Variable ::= Identifier

