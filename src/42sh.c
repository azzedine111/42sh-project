#include <err.h>
#include <unistd.h>

#include "funct/funct.h"
#include "io/cstream.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "utils/vec.h"
#include "variables/variables.h"

/**
 * \brief Parse the command line arguments
 * \return A character stream
 */
static struct cstream *parse_args(int argc, char *argv[])
{
    // If launched without argument, read the standard input
    if (argc == 1)
    {
        if (isatty(STDIN_FILENO))
            return cstream_readline_create();
        return cstream_file_create(stdin, /* fclose_on_free */ false);
    }
    // input as a string case

    if (strcmp(argv[1], "-c") == 0)
    {
        return cstream_string_create(argv[2]);
    }

    // 42sh FILENAME
    if (argc == 2)
    {
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            warn("failed to open input file");
            return NULL;
        }

        return cstream_file_create(fp, /* fclose_on_free */ true);
    }

    fprintf(stderr, "Usage: %s [COMMAND]\n", argv[0]);
    return NULL;
}

/**
 * \brief Read and print lines on newlines until EOF
 * \return An error code
 */
enum error read_print_loop(struct cstream *cs, struct vec *line)
{
    enum error error;

    while (true)
    {
        // Read the next character
        int c;
        if ((error = cstream_pop(cs, &c)))
            return error;

        // If a newline was met, parse the line

        if (c == EOF /*if interactif and == '\n'*/)
            break;
        else
            vec_push(line, c);
    }
    struct command_list *cl = malloc(sizeof(struct command_list));
    cl->node = NULL;
    struct lexer *lexer = lexer_new(vec_cstring(line));
    if (lexer->input[0] == '\0')
        errx(2, "Empty input !");
    int status = parse(cl, lexer);
    if (status != 0)
    {
        free_cl(cl);
        lexer_free(lexer);
        var_memory_free(variables);
        return 1;
    }
    // Evaluate the command list
    int temp = eval_cl(cl);
    if (temp != 0)
    {
        lexer_free(lexer);
        var_memory_free(variables);
        return temp;
    }
    lexer_free(lexer);
    vec_reset(line);
    var_memory_free(variables);
    return NO_ERROR;
}

int main(int argc, char *argv[])
{
    int rc;

    // Parse command line arguments and get an input stream
    struct cstream *cs;
    if ((cs = parse_args(argc, argv)) == NULL)
    {
        rc = 1;
        goto err_parse_args;
    }

    // Create a vector to hold the current line
    struct vec line;
    vec_init(&line);

    // Run the test loop
    rc = read_print_loop(cs, &line);
    if (rc != NO_ERROR)
    {
        cstream_free(cs);
        vec_destroy(&line);
        return rc;
        rc = read_print_loop(cs, &line);
        goto err_loop;
    }

    // Success
    rc = 0;

err_loop:
    cstream_free(cs);
    vec_destroy(&line);
err_parse_args:
    return rc;
}
