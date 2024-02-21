#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "common.h"
#include "error.h"
#include "argparse.h"
#include "lexer.h"
#include "syntax.h"
#include "qbe.h"

#define TUCKY_AUTHOR  "Jonah Hendler"
#define TUCKY_VERSION "0.1.0"
#define TUCKY_DATE    "Feb 19, 2024"

static void help(const TuckyArgParser parser) {
    printf("#######################################\n");
    printf("# Tucky v%s\n", TUCKY_VERSION);
    printf("# Author: %s\n", TUCKY_AUTHOR);
    printf("# Last Modified: %s\n", TUCKY_DATE);
    printf("#######################################\n");
    printArguments(parser);
}

TuckyArgParser* SAFE_parser = NULL;

int main(int argc, char** argv) {
    /*****************************************************************************/
    /* Argument Parsing */
    TuckyArgParser parser = newArgParser(argc, argv); SAFE_parser = &parser;
    addArgument(&parser, 'f', "file"   ,    AS_MANY, REQUIRED, "Input file path to be processed");
    addArgument(&parser, 'o', "out"    ,          1, REQUIRED, "Output file path to be generated");
    addArgument(&parser, 'v', "verbose", STORE_TRUE, OPTIONAL, "Enable verbose output");
    addArgument(&parser, 'g', "debug"  , STORE_TRUE, OPTIONAL, "Enable debug symbols in executable");
    addArgument(&parser, 'w', "werror" , STORE_TRUE, OPTIONAL, "Warnings are errors");
    addArgument(&parser, 'a', "ast"    , STORE_TRUE, OPTIONAL, "Dump abstract syntax tree");
    addArgument(&parser, 's', "asm"    , STORE_TRUE, OPTIONAL, "Skip linking and only dump assembly");

    parseArgs(parser);

    const bool help_set = getArgumentFromFlag(parser, 'h')->enabled;
    if (help_set) {
        help(parser);
        goto END;
    }

    /*****************************************************************************/
    /* AST Building */
    const char* file_path = getArgumentFromFlag(parser, 'f')->args->txt;
    FileLine file_as_lines = readFileAsLines(file_path);
    if (file_as_lines == NULL) {
        safeExit(TUCKY_ERRCO_EMPTY_FILE, "Cannot process a blank file.");
    }
    // dumpFileLines(file_as_lines);

    Token file_as_tokens = tokenizeFileLines(file_as_lines);
    // dumpTokens(file_as_tokens);

    SyntaxNode master_node = buildSyntaxTree(file_as_tokens);
    treeSyntaxNode(master_node, 0);

    /*****************************************************************************/
    /* Compilation to QBE */
    const char* output_path = getArgumentFromFlag(parser, 'o')->args->txt;
    char* qbe_path    = NULL;
    asprintf(&qbe_path, "%s.ssa", output_path);
    assert(qbe_path);

    compileToQBE(qbe_path, master_node);
    if (qbe_path) free(qbe_path);

    /*****************************************************************************/
    /* Linking QBE to Executable */
    // TODO:

    /*****************************************************************************/
    /* Cleanup */
    delSyntaxNode(&master_node);
    delFileLine  (&file_as_lines);

    /*****************************************************************************/
END:
    printf("\n");
    delArgParser(parser);
    return EXIT_SUCCESS;
}