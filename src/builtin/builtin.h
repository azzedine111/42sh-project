#pragma once

#include <err.h>
#include <unistd.h>

enum builtin_type // All builtin types
{
    BT_CD,
    BT_EXIT,
    BT_EXPORT,
    BT_CONTINUE,
    BT_BREAK,
    BT_DOT,
    BT_UNSET,
};

struct builtin
{
    enum builtin_type type;
    char *argv;
};

int change_dir(char *path);
