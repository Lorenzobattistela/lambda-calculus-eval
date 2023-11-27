#include "io.h"
#include "parser.h"
#include <stdio.h>

int main(void) {
  tokens_t parsed = parse_token("@");
  p_print_token(parsed);
  return 0;
}

// grammar
// Expression ::= LambdaExpression | Application | Variable
// LambdaExpression ::= "@" Identifier "." Expression
// Application ::= "(" Expression Expression ")"
// Variable ::= Identifier

