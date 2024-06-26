#include <assert.h>
#include <string.h>
#include "../common.h"

AstNode* create_var_node(const char* name, const char* type) {
    AstNode* node = malloc(sizeof(AstNode));
    node->type = VAR;
    node->node.variable = malloc(sizeof(Variable));
    node->node.variable->name = strdup(name);
    node->node.variable->type = type ? strdup(type) : NULL;
    return node;
}

AstNode* create_lambda_node(const char* param, const char* type, AstNode* body) {
    AstNode* node = malloc(sizeof(AstNode));
    node->type = LAMBDA_EXPR;
    node->node.lambda_expr = malloc(sizeof(LambdaExpression));
    node->node.lambda_expr->parameter = strdup(param);
    node->node.lambda_expr->type = strdup(type);
    node->node.lambda_expr->body = body;
    return node;
}

AstNode* create_application_node(AstNode* function, AstNode* argument) {
    AstNode* node = malloc(sizeof(AstNode));
    node->type = APPLICATION;
    node->node.application = malloc(sizeof(Application));
    node->node.application->function = function;
    node->node.application->argument = argument;
    return node;
}

void test_verbose_functions() {
    // Redirect stdout to a file for testing
    freopen("test_output.txt", "w", stdout);

    set_verbose(false);
    print_verbose("This should not be printed");

    set_verbose(true);
    print_verbose("This should be printed: %d", 42);

    // Reset stdout
    freopen("/dev/tty", "w", stdout);

    // Check the contents of the file
    FILE* file = fopen("test_output.txt", "r");
    char buffer[100] = {0};
    fgets(buffer, sizeof(buffer), file);
    fclose(file);

    assert(strcmp(buffer, "\nThis should be printed: 42") == 0);
}

// Test format function
void test_format() {
    char* result = format("Hello, %s! You are %d years old.", "John", 30);
    assert(strcmp(result, "Hello, John! You are 30 years old.") == 0);
    free(result);
}

void test_ast_to_string() {
    AstNode* var_node = create_var_node("x", "int");
    AstNode* lambda_node = create_lambda_node("y", "float", var_node);
    AstNode* app_node = create_application_node(lambda_node, var_node);

    char* result = ast_to_string(app_node);
    assert(strcmp(result, "((@y : float .(x : int) ) (x : int) ) ") == 0);

    free(result);
    // Free the AstNodes (implement a proper free_ast_node function for real use)
}

int main() {
    test_verbose_functions();
    test_format();
    test_ast_to_string();

    printf("All tests passed!\n");
    return 0;
}
