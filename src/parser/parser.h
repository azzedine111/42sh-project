#pragma once

#include <stdlib.h>
#include <string.h>

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../variables/variables.h"

/**
 * \brief Creates an AST by reading and interpreting tokens
 *   from the parser.
 */
void connexion_to_cl(struct command_list *cl, struct ast_node *node);
int parse(struct command_list *cl, struct lexer *lexer);
