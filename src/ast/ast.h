#pragma once

#include <unistd.h>

#include "../builtin/builtin.h"
#include "../lexer/lexer.h"

enum ast_type
{
    NODE_BUILTIN,
    NODE_UNTIL,
    NODE_WHILE,
    NODE_COMMAND,
    NODE_PIPE,
    NODE_NEGATION,
    NODE_IF,
    NODE_ELSE,
    NODE_ELIF,
    NODE_THEN,
    NODE_COND,
    NODE_AND,
    NODE_OR,
};

struct ast_command
{
    char **argv;
};

struct ast_pipe
{
    struct ast_command *left;
    struct ast_pipe *right;
};

struct ast_neg
{
    struct ast_pipe pipe;
};

struct ast_if
{
    enum ast_type type;
    struct compound_list *list;
    struct compound_list *if_cond;
    struct compound_list *if_then;
    struct compound_list *if_else;
};

struct compound_list
{
    struct ast_node *node;
};

struct node // Temporary definition of node
{
    enum ast_type type;
    char **argv;
};

struct linked_list
{
    struct node *node;
    struct linked_list *next;
};

struct ast_whun
{
    struct linked_list *compound1;
    struct linked_list *compound2;
};

struct ast_and_or
{
    enum ast_type type;
    struct ast_command *instr_left;
    struct ast_and_or *instr_right;
};

union ast_data
{
    struct ast_command ast_command;
    struct ast_if ast_if;
    struct ast_whun *ast_whun;
    struct builtin *builtin;
    struct ast_pipe ast_pipe;
    struct ast_neg ast_neg;
    struct ast_and_or *ast_and_or;
};

struct ast_node
{
    enum ast_type type; ///< The kind of node we're dealing with
    union ast_data data;
    struct ast_node *next;
};

struct command_list
{
    struct ast_node *node;
    struct ast_node *next;
};

struct ast_whun *make_whun_ast(struct lexer *lexer);

/**
 ** \brief Allocate a new ast with the given type
 */
struct ast_node *ast_new_command(struct lexer *lexer);
struct ast_node *ast_new_whun(struct lexer *lexer);
struct ast_node *ast_new_neg(struct lexer *lexer);
struct ast_node *ast_new_if(struct lexer *lexer);
struct ast_node *ast_new_cd(struct lexer *lexer, struct token *tok);
struct ast_node *ast_new_pipe(struct lexer *lexer, struct ast_node *command);
struct ast_node *ast_new_and(struct lexer *lexer, struct ast_node *command);
struct ast_node *ast_new_or(struct lexer *lexer, struct ast_node *command);
void write_in_list(struct lexer *lexer, struct compound_list *tmp);
void make_if_ast(struct lexer *lexer, struct ast_if *root);
struct builtin *make_cd_ast(struct lexer *lexer);
