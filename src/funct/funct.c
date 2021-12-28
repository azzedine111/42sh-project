#include "funct.h"

#include <fcntl.h>

int eval_cp(struct compound_list *cp)
{
    int rc = 0;
    struct ast_node *ptr = cp->node;
    while (ptr != NULL)
    {
        if (ptr->type == NODE_IF)
            rc = eval_if_ast(ptr->data.ast_if);
        else if (ptr->type == NODE_AND)
            eval_and(ptr->data.ast_and_or->instr_left->argv,
                     ptr->data.ast_and_or->instr_right->instr_left->argv);
        else if (ptr->type == NODE_OR)
            eval_or(ptr->data.ast_and_or->instr_left->argv,
                    ptr->data.ast_and_or->instr_right->instr_left->argv);
        else if (strcmp(ptr->data.ast_command.argv[0], "echo") == 0)
        {
            if (simple_echo(ptr->data.ast_command.argv) != 0)
            {
                return 1;
            }
        }
        else
            rc = simple_command(ptr->data.ast_command.argv);
        ptr = ptr->next;
    }
    return rc;
}

int eval_if_ast(struct ast_if node)
{
    if (eval_cp(node.if_cond) == 0)
        return eval_cp(node.if_then);
    else if (node.if_else != NULL)
        return eval_cp(node.if_else);
    return 0;
}

int eval_cl(struct command_list *cl)
{
    struct ast_node *ptr = cl->node;
    int rc = 0;
    while (ptr != NULL)
    {
        if (ptr->type == NODE_COMMAND)
        {
            if (strcmp(ptr->data.ast_command.argv[0], "echo") == 0)
                rc = simple_echo(ptr->data.ast_command.argv);
            else
                rc = simple_command(ptr->data.ast_command.argv);
        }
        else if (ptr->type == NODE_WHILE)
        {
            eval_while(ptr->data.ast_whun);
        }
        else if (ptr->type == NODE_UNTIL)
        {
            eval_until(ptr->data.ast_whun);
        }
        else if (ptr->type == NODE_BUILTIN)
        {
            if (ptr->data.builtin->type == BT_CD)
            {
                int status;
                status = change_dir(ptr->data.builtin->argv);
                if (status == 2 && ptr->next == NULL)
                    exit(2);
            }
        }
        else if (ptr->type == NODE_PIPE)
        {
            exec_pipe(ptr->data.ast_pipe.left->argv,
                      ptr->data.ast_pipe.right->left->argv);
        }
        else if (ptr->type == NODE_NEGATION)
            eval_negation(&ptr->data.ast_neg.pipe);
        else if (ptr->type == NODE_AND)
        {
            rc = eval_and(ptr->data.ast_and_or->instr_left->argv,
                          ptr->data.ast_and_or->instr_right->instr_left->argv);
        }
        else if (ptr->type == NODE_OR)
        {
            rc = eval_or(ptr->data.ast_and_or->instr_left->argv,
                         ptr->data.ast_and_or->instr_right->instr_left->argv);
        }
        else
        {
            rc = eval_if_ast(ptr->data.ast_if);
        }
        ptr = ptr->next;
        /*if (rc != 0)
        {
            return rc;
        }*/
    }
    free_cl(cl);
    return rc;
}

int exec_pipe(char **argv_left, char **argv_right)
{
    int fd[2];
    int tube = pipe(fd);
    if (tube == -1)
    {
        printf("pipe error");
        exit(1);
    }
    int pid_left = fork();
    if (pid_left == -1)
    {
        printf("pid_left error");
        exit(1);
    }
    if (pid_left == 0)
    {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(*argv_left, argv_left);
    }
    int pid_right = fork();
    if (pid_right == -1)
    {
        printf("pid_right error");
    }
    if (pid_right == 0)
    {
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(*argv_right, argv_right);
    }
    close(fd[0]);
    close(fd[1]);
    int status;
    waitpid(pid_left, &status, 0);
    waitpid(pid_right, &status, 0);
    return 0;
}

void my_print(char *str)
{
    int k = 0;
    while (str[k] != '\0')
    {
        if (str[k] != '\'' && str[k] != '"' && str[k] != '\\')
            putchar(str[k]);
        k++;
    }
}

void my_print_escapes(char *str)
{
    int k = 0;
    while (str[k] != '\0')
    {
        if (str[k] == '\\')
        {
            k++;
            if (str[k] == 'n')
                putchar('\n');
            else if (str[k] == 't')
                putchar('\t');
            else if (str[k] == '\\')
                putchar('\\');
        }
        else if (str[k] != '\'' && str[k] != '"')
            putchar(str[k]);
        k++;
    }
}

int check_e(char *str)
{
    for (size_t k = 0; k < strlen(str); k++)
        if (str[k] == 'e')
            return 1;
    return 0;
}

int no_n(char *str)
{
    for (size_t k = 0; k < strlen(str); k++)
        if (str[k] == 'n')
            return 0;
    return 1;
}

int simple_echo(char **node_argv)
{
    int k = 1;
    int presence_option = 0;
    // check options

    char *options = zalloc(100);
    int i = 1;
    int index = 0;
    while (node_argv[i] != NULL && node_argv[i][0] == '-')
    {
        if (node_argv[i][1] == 'n' || node_argv[i][1] == 'e')
        {
            options[index] = node_argv[i][1];
            index++;
        }
        i++;
    }
    while (node_argv[k] != NULL)
    {
        if (strcmp(node_argv[k], ">") == 0)
        {
            presence_option = 1;
            int stdout_dup = dup(STDOUT_FILENO);

            int fd = open(node_argv[k + 1], O_CREAT | O_TRUNC | O_WRONLY, 0755);

            if (fd < 0)
                return 1;

            if (dup2(fd, STDOUT_FILENO) == -1)
                return 1;

            for (int i = 1; i < k; i++)
            {
                if (i != 1)
                    putchar(' ');
                if (check_e(options))
                    my_print_escapes(node_argv[i]);
                else
                    my_print(node_argv[i]);
            }
            if (no_n(options))
                putchar('\n');
            fflush(stdout);

            dup2(stdout_dup, STDOUT_FILENO);
            close(stdout_dup);
        }
        k++;
    }
    if (presence_option == 0)
    {
        k = 1;
        while (strcmp(node_argv[k], "-e") == 0
               || strcmp(node_argv[k], "-n") == 0)
            k++;
        int start_point = k;
        while (node_argv[k] != NULL)
        {
            if (k != start_point)
                putchar(' ');
            if (check_e(options))
                my_print_escapes(node_argv[k]);
            else
                my_print(node_argv[k]);
            k++;
        }
        if (no_n(options))
            putchar('\n');
    }
    free(options);
    return 0;
}

int simple_command(char **node_argv)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        if (execvp(node_argv[0], node_argv) == -1)
            exit(WEXITSTATUS(status));
        exit(0);
    }
    waitpid(pid, &status, 0);
    if (WEXITSTATUS(status) > 2)
        return 127;
    return WEXITSTATUS(status);
}

void pretty_print_ast(struct ast_if *root)
{
    if (root->if_cond != NULL && root->type == NODE_IF) //"if" node case
    {
        printf("if { ");
        for (int i = 0; root->if_cond->node->data.ast_command.argv[i] != 0; i++)
        {
            printf("%s ", root->if_cond->node->data.ast_command.argv[i]);
        }
        printf("}");
    }
    if (root->if_cond != NULL && root->type == NODE_ELIF) //"elif" node case
    {
        printf("; elif { ");
        for (int i = 0; root->if_else->node->data.ast_command.argv[i] != NULL;
             i++)
        {
            printf("%s ", root->if_else->node->data.ast_command.argv[i]);
        }
        printf("}");
    }
    if (root->if_then != NULL && root->type == NODE_IF) //"then" node case
    {
        printf("; then { ");
        for (int i = 0; root->if_then->node->data.ast_command.argv[i] != NULL;
             i++)
        {
            printf("%s ", root->if_then->node->data.ast_command.argv[i]);
        }
        printf("}");
    }
    if (root->if_else != NULL) //"else" node case
    {
        printf("; else { ");
        for (int i = 0; root->if_else->node->data.ast_command.argv[i] != NULL;
             i++)
        {
            printf("%s ", root->if_else->node->data.ast_command.argv[i]);
        }
        printf("}");
    }
}

void free_if_ast(struct ast_if *root)
{
    if (root->if_cond != NULL) // If if_cond node is not NULL, free it
    {
        // to change
        free_cp_list(root->if_cond);
        root->if_cond = NULL;
    }
    if (root->if_then != NULL) // Do the same with if_then
    {
        free_cp_list(root->if_then);
        root->if_then = NULL;
    }
    if (root->if_else != NULL)
    {
        free_cp_list(root->if_else);
        root->if_else = NULL;
    }
}

void free_argv(char **node_argv)
{
    for (int i = 0; node_argv[i] != NULL; i++)
    {
        free(node_argv[i]);
    }
    free(node_argv);
}

void free_whun_ast(struct ast_whun *root)
{
    struct linked_list *tmp1 = root->compound1;
    struct linked_list *tmp2;
    while (tmp1 != NULL)
    {
        tmp2 = tmp1;
        free_argv(tmp2->node->argv);
        free(tmp2->node);
        tmp1 = tmp1->next;
        free(tmp2);
    }
    tmp1 = root->compound2;
    while (tmp1 != NULL)
    {
        tmp2 = tmp1;
        free_argv(tmp2->node->argv);
        free(tmp2->node);
        tmp1 = tmp1->next;
        free(tmp2);
    }
}

void free_pipe(struct ast_pipe *pipe)
{
    if (pipe == NULL)
        return;
    free_pipe(pipe->right);

    free_argv(pipe->left->argv);
    free(pipe->left);
    free(pipe);
}

void free_ast(struct ast_node *node)
{
    if (node == NULL)
        return;
    free_ast(node->next);
    if (node->type == NODE_IF)
    {
        free_if_ast(&node->data.ast_if);
    }
    else if (node->type == NODE_COMMAND)
    {
        free_argv(node->data.ast_command.argv);
    }
    else if (node->type == NODE_PIPE)
    {
        free_argv(node->data.ast_pipe.left->argv);
        free(node->data.ast_pipe.left);
        free_pipe(node->data.ast_pipe.right);
    }
    else if (node->type == NODE_NEGATION)
    {
        free_argv(node->data.ast_neg.pipe.left->argv);
        free(node->data.ast_neg.pipe.left);
    }
    else if (node->type == NODE_UNTIL || node->type == NODE_WHILE)
    {
        free_whun_ast(node->data.ast_whun);
        free(node->data.ast_whun);
    }
    else if (node->type == NODE_BUILTIN)
    {
        free(node->data.builtin->argv);
        free(node->data.builtin);
    }
    else if (node->type == NODE_AND || node->type == NODE_OR)
    {
        free_argv(node->data.ast_and_or->instr_left->argv);
        free_argv(node->data.ast_and_or->instr_right->instr_left->argv);
        free(node->data.ast_and_or->instr_left);
        free(node->data.ast_and_or->instr_right->instr_left);
        free(node->data.ast_and_or->instr_right);
        free(node->data.ast_and_or);
    }
    free(node);
}

void free_cl(struct command_list *cl)
{
    if (cl->node == NULL)
        return;
    free_ast(cl->node);
    free(cl);
}

int (*get_func(char *command))(
    char **) // Return a function pointer which match with the command
{
    int (*func)(char **);

    if (strcmp(command, "echo") == 0)
        func = simple_echo;

    else
        func = simple_command;

    return func;
}

int eval_negation(struct ast_pipe *pipe)
{
    if (strcmp(pipe->left->argv[0], "echo") == 0)
    {
        if (simple_echo(pipe->left->argv) == 0)
            return 1;
        return 0;
    }
    else
    {
        if (simple_command(pipe->left->argv) == 0)
            return 1;
        return 0;
    }
    return 2;
}

void eval_while(struct ast_whun *ast)
{
    int (*func1)(char **) = get_func(
        ast->compound1->node->argv[0]); // Initialize func1 wich depends on the
                                        // compound1 node argv
    int (*func2)(char **) =
        get_func(ast->compound2->node->argv[0]); // Same here with func2
    while (func1(ast->compound1->node->argv) == 0)
    {
        func2(ast->compound2->node->argv);
    }
}

void eval_until(struct ast_whun *ast)
{
    int (*func1)(char **) = get_func(
        ast->compound1->node->argv[0]); // Initialize func1 wich depends on the
                                        // compound1 node argv
    int (*func2)(char **) =
        get_func(ast->compound2->node->argv[0]); // Same here with func2
    while (func1(ast->compound1->node->argv) != 0)
    {
        func2(ast->compound2->node->argv);
    }
}

void free_cp_list(struct compound_list *cp)
{
    if (cp->node == NULL)
        return;
    free_ast(cp->node);
    free(cp);
}

int pre_eval_and_or(char **argv)
{
    if (strcmp(argv[0], "echo") == 0)
    {
        return simple_echo(argv);
    }
    else
        return simple_command(argv);
}

int eval_and(char **argv_left, char **argv_right)
{
    int rc = 0;
    if ((argv_left == NULL) || (argv_right == NULL))
        return 1;
    else if ((rc = pre_eval_and_or(argv_left)) != 0)
        return rc;
    else if ((rc = pre_eval_and_or(argv_right)) != 0)
    {
        return rc;
    }
    return 0;
}

int eval_or(char **argv_left, char **argv_right)
{
    if ((argv_left == NULL) && (argv_right == NULL))
        return 1;
    if (pre_eval_and_or(argv_left) != 0 && pre_eval_and_or(argv_right) != 0)
        return 1;
    return 0;
}
