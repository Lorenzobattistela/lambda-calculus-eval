#include "io.h"
#include "parser.h"
#include <stdio.h>

int main(void) {
  char token;
  FILE *in = get_file("expr.lambda", "r");

  while((token = next(in)) != (char) EOF && token != '\n') {
    tokens_t parsed = parse_token(token);
    if(parsed == ERROR) {
      printf("\nSYNTAX ERROR: TOKEN NOT ALLOWED: %c\n", token);
      return 1;
    }
    p_print_token(parsed);
  }

  return 0;
}

// grammar
// Expression ::= LambdaExpression | Application | Variable
// LambdaExpression ::= "@" Identifier "." Expression
// Application ::= "(" Expression Expression ")"
// Variable ::= Identifier

