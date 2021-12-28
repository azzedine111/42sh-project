#include "builtin.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int change_dir(char *path)
{
    char cur_path[100];
    char old_path[100];
    getcwd(old_path, sizeof(old_path));
    setenv("OLDPWD", old_path, 1);
    if (chdir(path) != 0)
    {
        warn("cd: %s", path);
        return 2;
    }
    getcwd(cur_path, sizeof(cur_path));
    setenv("PWD", cur_path, 1);
    return 0;
}
