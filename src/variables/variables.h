#ifndef VARIABLES_H
#define VARIABLES_H

struct variable
{
    char *name;
    char *value;
    struct variable *next;
};

extern struct variable *variables;

void var_memory_free(struct variable *var);
void add_var(char *name, char *value);
struct variable *copy_var(struct variable *var);
void new_variable(char *text);
char *find_var(char *str, char *res);
#endif
