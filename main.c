#include "./hash-table/hash_table.h"
#include "io.h"
#include "parser.h"
#include "reducer.h"
#include <stdio.h>

#define MAX_LAMBDA_LENGTH 1000
#define TMP_PATH "tmp/expr.lambda"
#define CONFIG_PATH "config"

typedef enum {
  APPLICATIVE, // always fully evaluate the arguments of a function before
               // evaluating the function itself
  NORMAL,      // reduction happens from the outside in
} reduction_order_t;

typedef struct {
  FILE *file;
  bool step_by_step_reduction;
  reduction_order_t reduction_order;
} Options;

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

bool get_reduction_step_option() {
  int option;
  while (true) {
    printf("Choose: \n[1] Normal reduction\n[2] Step by step reduction\n");
    scanf("%d", &option);
    while (getchar() != '\n')
      ;
    switch (option) {
    case 1: {
      return false;
    }
    case 2: {
      return true;
    }
    default:
      return false;
    }
  }
}

reduction_order_t get_reduction_order() {
  int option;
  while (true) {
    printf("Choose: \n[1] Normal order\n[2] Applicative Order\n");
    scanf("%d", &option);
    while (getchar() != '\n')
      ;
    switch (option) {
    case 1: {
      return NORMAL;
    }
    case 2: {
      return APPLICATIVE;
    }
    }
  }
}

// Options get_config_from_file() {}

Options cli() {
  FILE *f;
  int option;
  bool valid_option = false;
  bool step_by_step_reduction = get_reduction_step_option();
  reduction_order_t reduction_order = get_reduction_order();

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
  Options opt;
  opt.file = f;
  opt.step_by_step_reduction = step_by_step_reduction;
  opt.reduction_order = reduction_order;
  return opt;
}

int main(void) {
  Options opt = cli();
  FILE *in = opt.file;
  HANDLE_NULL(in);
  HashTable *table = createHashTable();

  AstNode *res = parse_expression(table, in, next(in));
  print_ast(res);
  printf("\n");

  expand_definitions(table, res);
  print_ast(res);

  AstNode *red = reduce_ast(res);
  printf("\nReduced ast:\n");
  char *reduced_ast_str = ast_to_string(red);
  printf("%s\n", reduced_ast_str);

  free_ast(res);
  destroyHashTable(table);
  close_file(in);
  return 1;
}

// grammar
// Expression ::= LambdaExpression | Application | Variable
// LambdaExpression ::= "@" Identifier "." Expression
// Application ::= "(" Expression Expression ")"
// Variable ::= Identifier
