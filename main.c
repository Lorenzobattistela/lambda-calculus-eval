#include "common.h"
#include "io.h"
#include "parser.h"
#include "reducer.h"
#include <stdio.h>

#define MAX_LAMBDA_LENGTH 1000
#define TMP_PATH "tmp/expr.lambda"

char *get_str_input(char *input_msg, char *error_msg) {
  char *input = malloc(MAX_LAMBDA_LENGTH + 1 * sizeof(char));
  HANDLE_NULL(input);

  printf("%s", input_msg);

  if (fgets(input, MAX_LAMBDA_LENGTH + 1, stdin) == NULL) {
    fprintf(stderr, "%s\n", error_msg);
    free(input);
    exit(EXIT_FAILURE);
  }

  size_t len = strlen(input);
  if (len > 0 && input[len - 1] == '\n') {
    input[len - 1] = '\0';
  }

  return input;
}

char *get_lambda_expr_input() {
  char *lambda_expr =
      get_str_input("Enter the lambda expression: ",
                    "ERROR: failed to read the lambda expression.");

  FILE *file = create_file(TMP_PATH);
  write_to_file(file, lambda_expr);
  close_file(file);
  return TMP_PATH;
}

char *get_file_path_input() {
  char *file_path = get_str_input("Enter the file path for your expression: ",
                                  "ERROR: failed to read file path.");
  return file_path;
}

FILE *cli() {
  FILE *f;
  int option;
  bool valid_option = false;

  while (!valid_option) {

    printf("Choose your option:\n[1] Write input\n[2] File input\n");

    scanf("%d", &option);

    while (getchar() != '\n')
      ;
    switch (option) {
    case 1: {
      char *path = get_lambda_expr_input();
      f = get_file(path, "r");
      valid_option = true;
      break;
    }
    case 2: {
      char *path = get_file_path_input();
      f = get_file(path, "r");
      free(path);
      valid_option = true;
      break;
    }
    default:
      printf("Invalid input. Try again\n");
      break;
    }
  }
  return f;
}

int main(void) {
  FILE *in = cli();
  HANDLE_NULL(in);
  AstNode *res = parse_expression(in, next(in));
  print_ast(res);
  close_file(in);
  return 1;
}

// grammar
// Expression ::= LambdaExpression | Application | Variable
// LambdaExpression ::= "@" Identifier "." Expression
// Application ::= "(" Expression Expression ")"
// Variable ::= Identifier
