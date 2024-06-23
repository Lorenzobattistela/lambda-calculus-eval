#include "typechecker.h"

char *create_function_type(char *param_type, char *return_type) {
    int len = strlen(param_type) + strlen(return_type) + 5;
    char *type = malloc(len);
    snprintf(type, len, "%s -> %s", param_type, return_type);
    return type;
}

char *check_type(AstNode *node, TypeEnv *env) {
    switch (node->type) {
        case LAMBDA_EXPR:
            return check_lambda(node->node.lambda_expr, env);
        case APPLICATION:
            return check_application(node->node.application, env);
        case VAR:
            return check_variable(node->node.variable, env);
        default:
            fprintf(stderr, "Unknown node type\n");
            return NULL;
    }
}

char *check_lambda(LambdaExpression *lambda, TypeEnv *env) {
    TypeEnv *new_env = NULL;
    add_to_env(&new_env, lambda->parameter, lambda->type);
    new_env->next = env;

    char *body_type = check_type(lambda->body, new_env);
    if (body_type == NULL) {
        return NULL;
    }

    char *lambda_type = create_function_type(lambda->type, body_type);
    free(body_type);
    return lambda_type;
}

char *check_application(Application *app, TypeEnv *env) {
    char *func_type = check_type(app->function, env);
    if (func_type == NULL) {
        return NULL;
    }

    char *arg_type = check_type(app->argument, env);
    if (arg_type == NULL) {
        free(func_type);
        return NULL;
    }

    char *arrow_pos = strstr(func_type, "->");
    if (arrow_pos == NULL) {
        fprintf(stderr, "Invalid function type: %s\n", func_type);
        free(func_type);
        free(arg_type);
        return NULL;
    }

    int param_type_len = arrow_pos - func_type;
    char param_type[param_type_len + 1];
    strncpy(param_type, func_type, param_type_len);
    param_type[param_type_len] = '\0';

    if (strcmp(param_type, arg_type) != 0) {
        fprintf(stderr, "Type mismatch: expected %s, got %s\n", param_type, arg_type);
        free(func_type);
        free(arg_type);
        return NULL;
    }

    char *return_type = strdup(arrow_pos + 2);
    free(func_type);
    free(arg_type);
    return return_type;
}

char *check_variable(Variable *var, TypeEnv *env) {
    char *type = lookup_type(env, var->name);
    if (type == NULL) {
        fprintf(stderr, "Undefined variable: %s\n", var->name);
        return NULL;
    }
    return strdup(type);
}

void add_to_env(TypeEnv **env, char *var_name, char *type) {
    TypeEnv *new_env = malloc(sizeof(TypeEnv));
    new_env->var_name = strdup(var_name);
    new_env->type = strdup(type);
    new_env->next = *env;
    *env = new_env;
}

char *lookup_type(TypeEnv *env, char *var_name) {
    while (env != NULL) {
        if (strcmp(env->var_name, var_name) == 0) {
            return env->type;
        }
        env = env->next;
    }
    return NULL;
}
