#include "lexer.h"

struct lexer *lexer_new(const char *input)
{
    struct lexer *new_lex = xmalloc(sizeof(struct lexer));
    new_lex->input = input;
    new_lex->pos = 0;
    new_lex->current_tok = NULL;
    return new_lex;
}

struct token *lexer_peek(struct lexer *lexer)
{
    char *str;
    while (lexer->input[lexer->pos] == ' ' || lexer->input[lexer->pos] == '\t')
    {
        lexer->pos++;
    }
    char *cur_str = (char *)(lexer->input + lexer->pos);

    if (cur_str[0] == ';' || cur_str[0] == '\n') // Stop immediately after
    {
        str = strndup(cur_str, 1);
    }
    else
    {
        int k = 0;
        while (cur_str[k] != ' ' && cur_str[k] != '\0' && cur_str[k] != '\n'
               && cur_str[k] != ';')
        {
            if (cur_str[k] != '\0' && cur_str[k] == '"')
            {
                k++;
                while (cur_str[k] != '\0' && cur_str[k] != '"')
                {
                    k++;
                    if (cur_str[k] == '\0')
                        break; // error case
                }
            }
            else if (cur_str[k] != '\0'
                     && cur_str[k] == '\'') // this is the condition for the ""
            {
                k++;
                while (cur_str[k] != '\0' && cur_str[k] != '\'')
                {
                    k++;
                    if (cur_str[k] == '\0')
                        break; // error case
                }
            }
            k++;
        }
        str = strndup(cur_str, k);
    }
    struct token_model exprs[] = {
        { ">", TOKEN_CHEVRON },  { ";", TOKEN_SEMICOLON },
        { "\n", TOKEN_NEWLINE }, { "'", TOKEN_SINGLEQUOTE },
        { "\0", TOKEN_EOF },     { "|", TOKEN_PIPE },
        { "&&", TOKEN_AND },     { "||", TOKEN_OR }
    };

    struct token *token = xmalloc(sizeof(struct token));
    token->type = TOKEN_WORDS;
    token->word = str;
    for (unsigned i = 0; i < sizeof(exprs) / sizeof(*exprs); ++i)
    {
        if (strcmp(str, exprs[i].str) == 0)
        {
            token->type = exprs[i].type;
            return token;
        }
    }
    return token;
}

void lexer_free(struct lexer *lexer)
{
    free(lexer);
}

struct token *lexer_pop(struct lexer *lexer)
{
    struct token *rtrn_tok = lexer_peek(lexer);
    lexer->current_tok = rtrn_tok;
    lexer->pos += strlen(rtrn_tok->word);
    return rtrn_tok;
}
