#include "parser.h"

#include <err.h>
#include <fnmatch.h>
#include <stdbool.h>

int parse_exp(struct command_list *cl, struct lexer *lexer);

/**
 * \brief Parse either an expression, or nothing
 *
 * input:  EOF  |  exp EOF
 */
int parse(struct command_list *cl, struct lexer *lexer)
{
    // If we're at the end of file, there's no input
    struct token *tok = lexer_peek(lexer);
    if (tok->type == TOKEN_EOF)
    {
        token_free(tok);
        return 0;
    }
    token_free(tok);

    // try to parse an expression. if an error occured, free the
    // produced ast and return the same error code
    int status = parse_exp(cl, lexer);
    if (status != 0)
        return 1;

    // once parsing the expression is done, we should have
    // reached the end of file.
    struct token *token = lexer_peek(lexer);
    if (token->type == TOKEN_EOF)
    {
        token_free(token);
        return 0;
    }
    token_free(token);

    // if we didn't reach the end of file, it's an error
    return 1;
}

void connexion_to_cl(struct command_list *cl, struct ast_node *new_node)
{
    // Connect the node with the linked list node of the command list
    if (cl->node == NULL)
    {
        cl->node = new_node;
    }
    else
    {
        struct ast_node *ptr = cl->node;
        while (ptr->next != NULL)
            ptr = ptr->next;
        ptr->next = new_node;
    }
}

int parse_exp(struct command_list *cl, struct lexer *lexer)
{
    while (true)
    {
        // take the next token.
        struct token *tok = lexer_pop(lexer);

        // make the new ast node and discard the token
        if (tok->type == TOKEN_WORDS)
        {
            if (strcmp(tok->word, "if") == 0)
            {
                connexion_to_cl(cl, ast_new_if(lexer));
                token_free(tok);
            }
            else if (strcmp(tok->word, "while") == 0
                     || strcmp(tok->word, "until") == 0)
            {
                connexion_to_cl(cl, ast_new_whun(lexer));
                token_free(tok);
            }
            else if (strcmp(tok->word, "cd") == 0)
            {
                connexion_to_cl(cl, ast_new_cd(lexer, tok));
            }
            else if (fnmatch("*=*", tok->word, 0) == 0)
            {
                new_variable(tok->word);
                token_free(tok);
            }
            else if (strcmp(tok->word, "!") == 0)
            {
                connexion_to_cl(cl, ast_new_neg(lexer));
                token_free(tok);
            }
            else
            {
                struct ast_node *command_o_pipe = ast_new_command(lexer);
                connexion_to_cl(cl, command_o_pipe);
                token_free(tok);
            }
        }
        else if (tok->type == TOKEN_SEMICOLON || tok->type == TOKEN_NEWLINE)
        {
            token_free(tok);
            continue;
        }
        else
        {
            token_free(tok);
            break;
        }
    }
    return 0;
}
