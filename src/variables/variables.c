#include "variables.h"

#include <stdio.h>
#include <stdlib.h>

#include "../alloc.h"

struct variable *variables = NULL;

struct variable *var_memory_alloc(void)
{
    struct variable *var = NULL;
    var = zalloc(sizeof(struct variable));
    var->name = zalloc(100);
    var->value = zalloc(100);
    var->next = NULL;
    return var;
}

void var_memory_free(struct variable *var)
{
    struct variable *tmp;
    while (var != NULL)
    {
        tmp = var;
        var = var->next;
        free(tmp->name);
        free(tmp->value);
        free(tmp);
    }
}

void add_var(char *name, char *value)
{
    struct variable *temp_var = var_memory_alloc();
    strcpy(temp_var->name, name);
    strcpy(temp_var->value, value);
    temp_var->next = copy_var(variables);
    // copy every element of temp_var in variables
    variables = copy_var(temp_var);
    var_memory_free(temp_var);
}

struct variable *copy_var(struct variable *var)
{
    if (var == NULL)
        return NULL;
    else
    {
        struct variable *new_var = var_memory_alloc();
        strcpy(new_var->name, var->name);
        strcpy(new_var->value, var->value);
        new_var->next = copy_var(var->next);
        return new_var;
    }
}

void new_variable(char *text)
{
    if (variables == NULL)
        variables = var_memory_alloc();

    char variable[100];
    char *result;

    unsigned int k;
    for (k = 0; k < sizeof(text); k++)
        if (text[k] == '=')
            break;
    strncpy(variable, &text[0], k);
    variable[k] = '\0';

    result = strchr(text, '=');
    result += 1;

    add_var(variable, result);
}

char *find_var(char *str, char *res)
{
    struct variable *cur = variables;
    while (cur != NULL)
    {
        if (strcmp(cur->name, str) == 0)
            strcpy(res, cur->value);
        cur = cur->next;
    }
    return res;
}
