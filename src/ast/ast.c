#include "ast.h"

#include <fnmatch.h>

#include "../utils/alloc.h"
#include "../variables/variables.h"

struct ast_node *ast_new_command(struct lexer *lexer)
{
    struct ast_node *ast = zalloc(sizeof(struct ast_node));

    ast->type = NODE_COMMAND;
    ast->data.ast_command.argv = zalloc(2048 * sizeof(char *));

    ast->data.ast_command.argv[0] =
        xmalloc(strlen(lexer->current_tok->word) * 2);
    ast->data.ast_command.argv[0] =
        strcpy(ast->data.ast_command.argv[0], lexer->current_tok->word);

    // Fill the arguments list
    int i = 1;
    struct token *token = lexer_pop(lexer);
    while (token->type == TOKEN_WORDS || token->type == TOKEN_CHEVRON)

    {
        char *word = zalloc(50);
        char *res = zalloc(50);
        // extension des variables
        if (fnmatch("$*", token->word, 0) == 0)
        {
            strcpy(word, find_var(token->word + 1, res));
        }
        else
            strcpy(word, token->word);
        ast->data.ast_command.argv[i] = xmalloc(strlen(word) * 2);
        ast->data.ast_command.argv[i] =
            strcpy(ast->data.ast_command.argv[i], word);

        token_free(token);
        token = lexer_pop(lexer);
        i++;
        free(word);
        free(res);
    }
    if (token->type == TOKEN_PIPE)
    {
        struct ast_node *pipe = ast_new_pipe(lexer, ast);
        token_free(token);
        return pipe;
    }
    else if (token->type == TOKEN_AND)
    {
        struct ast_node *and = ast_new_and(lexer, ast);
        token_free(token);
        return and;
    }
    else if (token->type == TOKEN_OR)
    {
        struct ast_node * or = ast_new_or(lexer, ast);
        token_free(token);
        return or ;
    }
    token_free(token);

    // Set the stop condition
    ast->next = NULL;

    return ast;
}

struct ast_pipe *make_pipe(struct lexer *lexer)
{
    struct ast_pipe *pipe = zalloc(sizeof(struct ast_pipe));

    pipe->left = zalloc(sizeof(struct ast_command));
    pipe->left->argv = zalloc(50 * sizeof(char *));
    pipe->left->argv[0] = zalloc(strlen(lexer->current_tok->word) * 2);
    pipe->left->argv[0] = strcpy(pipe->left->argv[0], lexer->current_tok->word);

    // Fill the arguments list
    int i = 1;
    struct token *token = lexer_pop(lexer);
    while (token->type == TOKEN_WORDS || token->type == TOKEN_CHEVRON)
    {
        pipe->left->argv[i] = xmalloc(strlen(token->word) * 2);
        pipe->left->argv[i] = strcpy(pipe->left->argv[i], token->word);

        token_free(token);
        token = lexer_pop(lexer);
        i++;
    }
    if (token->type == TOKEN_PIPE)
    {
        struct token *tok = lexer_pop(lexer);
        pipe->right = make_pipe(lexer);
        token_free(tok);
    }
    else
    {
        pipe->right = NULL;
    }
    token_free(token);
    return pipe;
}

struct ast_node *ast_new_pipe(struct lexer *lexer, struct ast_node *command)
{
    struct ast_node *ast = zalloc(sizeof(struct ast_node));

    ast->type = NODE_PIPE;
    ast->data.ast_pipe.left = zalloc(sizeof(struct ast_command));
    *ast->data.ast_pipe.left = command->data.ast_command;
    free(command);
    struct token *tok = lexer_pop(lexer);

    ast->data.ast_pipe.right = make_pipe(lexer);

    token_free(tok);
    // Set the stop condition
    ast->next = NULL;

    return ast;
}

struct ast_and_or *make_and(struct lexer *lexer)
{
    struct ast_and_or *and = zalloc(sizeof(struct ast_and_or));

    and->instr_left = zalloc(sizeof(struct ast_command));
    and->instr_left->argv = zalloc(50 * sizeof(char *));
    and->instr_left->argv[0] = zalloc(strlen(lexer->current_tok->word) * 10);
    and->instr_left->argv[0] =
        strcpy(and->instr_left->argv[0], lexer->current_tok->word);

    int i = 1;
    struct token *tok = lexer_pop(lexer);
    while (tok->type == TOKEN_WORDS)
    {
        and->instr_left->argv[i] = xmalloc(strlen(tok->word) * 2);
        and->instr_left->argv[i] = strcpy(and->instr_left->argv[i], tok->word);
        token_free(tok);
        tok = lexer_pop(lexer);
        i++;
    }
    if (tok->type == TOKEN_AND)
    {
        tok = lexer_pop(lexer);
        and->instr_right = make_and(lexer);
        free(lexer);
    }
    else
    {
        and->instr_right = NULL;
    }
    token_free(tok);
    return and;
}

struct ast_node *ast_new_and(struct lexer *lexer, struct ast_node *command)
{
    struct ast_node *ast = zalloc(sizeof(struct ast_node));

    ast->type = NODE_AND;
    ast->data.ast_and_or = zalloc(sizeof(struct ast_and_or));
    ast->data.ast_and_or->instr_left = zalloc(sizeof(struct ast_command));
    *ast->data.ast_and_or->instr_left = command->data.ast_command;
    free(command);
    struct token *tok = lexer_pop(lexer);

    ast->data.ast_and_or->instr_right = make_and(lexer);

    token_free(tok);
    ast->next = NULL;
    return ast;
}

struct ast_and_or *make_or(struct lexer *lexer)
{
    struct ast_and_or * or = zalloc(sizeof(struct ast_and_or));

    or->instr_left = zalloc(sizeof(struct ast_command));
    or->instr_left->argv = zalloc(50 * sizeof(char *));
    or->instr_left->argv[0] = zalloc(strlen(lexer->current_tok->word) * 10);
    or->instr_left->argv[0] =
        strcpy(or->instr_left->argv[0], lexer->current_tok->word);

    int i = 1;
    struct token *tok = lexer_pop(lexer);
    while (tok->type == TOKEN_WORDS)
    {
        or->instr_left->argv[i] = xmalloc(strlen(tok->word) * 2);
        or->instr_left->argv[i] = strcpy(or->instr_left->argv[i], tok->word);
        token_free(tok);
        tok = lexer_pop(lexer);
        i++;
    }
    if (tok->type == TOKEN_OR)
    {
        tok = lexer_pop(lexer);
        or->instr_right = make_and(lexer);
        free(lexer);
    }
    else
    {
        or->instr_right = NULL;
    }
    token_free(tok);
    return or ;
}

struct ast_node *ast_new_or(struct lexer *lexer, struct ast_node *command)
{
    struct ast_node *ast = zalloc(sizeof(struct ast_node));

    ast->type = NODE_OR;
    ast->data.ast_and_or = zalloc(sizeof(struct ast_and_or));
    ast->data.ast_and_or->instr_left = zalloc(sizeof(struct ast_command));
    *ast->data.ast_and_or->instr_left = command->data.ast_command;
    free(command);
    struct token *tok = lexer_pop(lexer);

    ast->data.ast_and_or->instr_right = make_or(lexer);

    token_free(tok);
    ast->next = NULL;
    return ast;
}

struct ast_node *ast_new_neg(struct lexer *lexer)
{
    struct ast_node *ast = zalloc(sizeof(struct ast_node));

    ast->type = NODE_NEGATION;
    ast->data.ast_neg.pipe.left = zalloc(sizeof(struct ast_node));
    struct token *tok = lexer_pop(lexer);
    struct ast_node *command = ast_new_command(lexer);
    *ast->data.ast_neg.pipe.left = command->data.ast_command;
    free(command);
    free(tok);
    return ast;
}

void write_in_list(struct lexer *lexer, struct compound_list *tmp)
{
    if (strcmp(lexer->current_tok->word, "if") == 0)
    {
        tmp->node = ast_new_if(lexer);
    }
    else if (tmp->node == NULL)
    {
        tmp->node = ast_new_command(lexer);
    }
    else
    {
        struct ast_node *ptr = tmp->node;
        while (ptr->next != NULL)
            ptr = ptr->next;
        ptr->next = ast_new_command(lexer);
    }
}

void make_if_ast(struct lexer *lexer, struct ast_if *root)
{
    // Switch after the "if"
    struct token *token = lexer_pop(lexer);
    root->if_cond = zalloc(sizeof(struct compound_list));
    write_in_list(lexer, root->if_cond); // Write into the compound list;

    token_free(token);
    token = lexer_pop(lexer); // Switch after the ";" token

    if (strcmp(token->word, "then") != 0)
        errx(2, "Expected token 'then' after ;");
    token_free(token);

    token = lexer_pop(lexer); // Switch after the "then" token

    // Pass the newline if there are one after then "then"
    if (token->type == TOKEN_NEWLINE)
    {
        token_free(token);
        token = lexer_pop(lexer);
    }
    root->if_then = zalloc(sizeof(struct compound_list));
    while (strcmp(lexer->current_tok->word, "fi") != 0
           && strcmp(lexer->current_tok->word, "else") != 0)
    {
        write_in_list(lexer, root->if_then);
        token_free(token);
        token = lexer_pop(lexer);

        // In the case ';' follow by a newline, pass it
        if (token->type == TOKEN_NEWLINE)
        {
            token_free(token);
            token = lexer_pop(lexer);
        }
        if (token->type == TOKEN_EOF)
            errx(2, "Expected token fi");
    }

    // Do the same with else, if there are one
    if (strcmp(lexer->current_tok->word, "else") == 0)
    {
        token_free(token);
        token = lexer_pop(lexer);
        root->if_else = zalloc(sizeof(struct compound_list));

        while (strcmp(lexer->current_tok->word, "fi") != 0)
        {
            write_in_list(lexer, root->if_else);
            token_free(token);
            token = lexer_pop(lexer);

            if (token->type == TOKEN_NEWLINE || token->type == TOKEN_SEMICOLON)
            {
                token_free(token);
                token = lexer_pop(lexer);
            }
            if (token->type == TOKEN_EOF)
                errx(2, "Expected token fi");
        }
    }
    token_free(token);
    token = lexer_peek(lexer);

    if (token->type == TOKEN_SEMICOLON || token->type == TOKEN_NEWLINE)
    {
        token_free(token);
        token = lexer_pop(lexer);
    }
    token_free(token);
}

struct ast_node *ast_new_if(struct lexer *lexer)
{
    struct ast_node *ast = zalloc(sizeof(struct ast_node));
    ast->type = NODE_IF;
    ast->data.ast_if.type = NODE_IF;

    make_if_ast(lexer, &ast->data.ast_if);

    return ast;
}

struct ast_node *ast_new_whun(struct lexer *lexer)
{
    struct ast_node *ast = zalloc(sizeof(struct ast_node));

    if (strcmp(lexer->current_tok->word, "while") == 0)
        ast->type = NODE_WHILE;
    else if (strcmp(lexer->current_tok->word, "until") == 0)
        ast->type = NODE_UNTIL;

    struct ast_whun *root = make_whun_ast(lexer);

    ast->data.ast_whun = root;

    return ast;
}

struct ast_node *ast_new_cd(struct lexer *lexer, struct token *tok)
{
    struct ast_node *ast = zalloc(sizeof(struct ast_node));

    ast->type = NODE_BUILTIN;

    struct builtin *cd = make_cd_ast(lexer);

    ast->data.builtin = cd;

    token_free(tok);

    return ast;
}

struct builtin *make_cd_ast(struct lexer *lexer)
{
    struct builtin *cd = malloc(sizeof(struct builtin));
    struct token *token;
    cd->type = BT_CD;
    token = lexer_pop(lexer);

    if (token->type == TOKEN_SEMICOLON || token->type == TOKEN_EOF)
    {
        char *home = getenv("HOME");
        cd->argv = xmalloc(strlen(home) + 1);
        strcpy(cd->argv, home);
    }
    else
    {
        cd->argv = xmalloc(strlen(lexer->current_tok->word) + 1);
        strcpy(cd->argv, lexer->current_tok->word);
    }

    token_free(token);
    return cd;
}

struct ast_whun *
new_whun_root(void) // Make and return a new empty ast_whun object
{
    struct ast_whun *root = zalloc(sizeof(struct ast_whun));
    root->compound1 = zalloc(sizeof(struct linked_list));
    root->compound2 = zalloc(sizeof(struct linked_list));
    root->compound1->next = NULL;
    root->compound2->next = NULL;
    root->compound1->node = NULL;
    root->compound2->node = NULL;
    return root;
}

void whun_compound1(struct lexer *lexer, struct linked_list *compound1,
                    int index)
{ // Make recursively the first ast_whun compound list
    struct token *token = lexer->current_tok;
    struct token *next_token = lexer_peek(lexer);

    if (token->type == TOKEN_NEWLINE)
    {
        while (token->type == TOKEN_NEWLINE
               && strcmp(next_token->word, "do") != 0)
        {
            token_free(token);
            token_free(next_token);
            lexer_pop(lexer);
            token = lexer->current_tok;
            next_token = lexer_peek(lexer);
        }
    }

    if ((strcmp(token->word, ";") == 0 && strcmp(next_token->word, "do") == 0)
        || (token->type == TOKEN_NEWLINE
            && strcmp(next_token->word, "do") == 0)) // Stop condition
    {
        while (token->type == TOKEN_NEWLINE
               && strcmp(next_token->word, "do") != 0)
        {
            token_free(token);
            token_free(next_token);
            lexer_pop(lexer);
            token = lexer->current_tok;
            next_token = lexer_peek(lexer);
        }
    }

    if ((strcmp(token->word, ";") == 0 && strcmp(next_token->word, "do") == 0)
        || (token->type == TOKEN_NEWLINE
            && strcmp(next_token->word, "do") == 0)) // Stop condition
    {
        token_free(token);
        token_free(next_token);
        return;
    }

    if (token->type == TOKEN_EOF) // Token error case
        errx(2, "syntax error near unexpected token `done'");

    if (compound1->node == NULL) // Make a node if it does not exist
    {
        compound1->node = zalloc(sizeof(struct node));
        compound1->node->type = NODE_COMMAND;
        compound1->node->argv = zalloc(100 * sizeof(char *));
    }

    compound1->node->argv[index] = zalloc(strlen(token->word) * 2);
    compound1->node->argv[index] =
        strcpy(compound1->node->argv[index], token->word);

    token_free(token);
    token_free(next_token);

    lexer_pop(lexer);
    index++;

    whun_compound1(lexer, compound1, index); // Do a recursion
}

void whun_compound2(struct lexer *lexer, struct linked_list *compound2,
                    int index)
{ // Make recursively the first ast_whun compound list
    struct token *token = lexer->current_tok;
    struct token *next_token = lexer_peek(lexer);

    if (token->type == TOKEN_NEWLINE)
    {
        while (token->type == TOKEN_NEWLINE
               && strcmp(next_token->word, "done") != 0)
        {
            token_free(token);
            token_free(next_token);
            lexer_pop(lexer);
            token = lexer->current_tok;
            next_token = lexer_peek(lexer);
        }
    }

    if ((strcmp(token->word, ";") == 0 && strcmp(next_token->word, "done") == 0)
        || (token->type == TOKEN_NEWLINE
            && strcmp(next_token->word, "done") == 0))
    {
        token_free(token);
        token_free(next_token);
        return;
    }

    if (token->type == TOKEN_EOF) // Token error case
        errx(2, "syntax error expected token `done'");

    if (compound2->node == NULL)
    {
        compound2->node = zalloc(sizeof(struct node));
        compound2->node->type = NODE_COMMAND;
        compound2->node->argv = zalloc(100 * sizeof(char *));
    }

    compound2->node->argv[index] = zalloc(strlen(token->word) * 2);
    compound2->node->argv[index] =
        strcpy(compound2->node->argv[index], token->word);

    token_free(token);
    token_free(next_token);

    lexer_pop(lexer);
    index++;

    whun_compound2(lexer, compound2, index);
}

struct ast_whun *make_whun_ast(struct lexer *lexer)
{ // Make and return the entire ast_whun object
    struct ast_whun *root = new_whun_root();
    struct token *token;
    struct token *next_token;
    token = lexer_pop(lexer);

    if (strcmp(token->word, ";") == 0)
    {
        token_free(token);
        errx(2, "syntax error near unexpected token `;'");
    }

    whun_compound1(lexer, root->compound1, 0);

    token = lexer_pop(lexer);
    next_token = lexer_peek(lexer);

    if (next_token->type == TOKEN_SEMICOLON
        || next_token->type == TOKEN_NEWLINE)
    {
        token_free(token);
        token_free(next_token);
        errx(2, "Syntax error: %c%c%c unexpected", '"', ';', '"');
    }

    token_free(token);
    token_free(next_token);
    token = lexer_pop(lexer);

    if (token->type == TOKEN_NEWLINE)
    {
        token_free(token);
        token = lexer_pop(lexer);
    }

    whun_compound2(lexer, root->compound2, 0);
    token = lexer_pop(lexer);
    token_free(token);

    return root;
}
