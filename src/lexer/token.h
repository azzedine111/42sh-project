#pragma once

#include <dirent.h>
#include <unistd.h>

enum token_type
{
  TOKEN_SEMICOLON,
  TOKEN_NEWLINE,
  TOKEN_SINGLEQUOTE,
  TOKEN_PIPE,
  TOKEN_EXCL,
  TOKEN_WORDS,
  TOKEN_EOF,
  TOKEN_CHEVRON,
  TOKEN_ERROR,
  TOKEN_AND,
  TOKEN_OR
};

struct token
{
    enum token_type type;
    char *word; // if the token type is TOKEN_WORDS
};

struct token_model
{
    const char *str;
    enum token_type type;
};

struct token *token_new(enum token_type type);

void token_free(struct token *token);
