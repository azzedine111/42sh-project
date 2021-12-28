#pragma once

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../variables/variables.h"

void pretty_print_ast(struct ast_if *root);
void free_if_ast(struct ast_if *root);

int simple_echo(char **node_argv);
int simple_command(char **node_argv);
int exec_pipe(char **argv_left, char **argv_right);

int eval_if_ast(struct ast_if node);
int eval_cl(struct command_list *cl);
void free_argv(char **node_argv);
void free_ast(struct ast_node *node);
void free_cl(struct command_list *cl);
int eval_negation(struct ast_pipe *pipe);
void eval_while(struct ast_whun *ast);
void eval_until(struct ast_whun *ast);
void free_cp_list(struct compound_list *cp);
int eval_and(char **argv_left, char **argv_right);
int eval_or(char **argv_left, char **argv_right);
