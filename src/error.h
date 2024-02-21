#ifndef TUCKY_ERROR_H
#define TUCKY_ERROR_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    TUCKY_ERRCO_NONE=0,

/* Command Line Argument Errors */
    TUCKY_ERRCO_FILE_NOT_FOUND,
    TUCKY_ERRCO_BAD_ARGUMENT,
    TUCKY_ERRCO_MISSING_ARG,

/* Lexer Errors */
    TUCKY_ERRCO_BAD_NUM_FORMAT,
    TUCKY_ERRCO_EMPTY_FILE,

/* Compiler Errors */
    TUCKLY_ERRCO_BAD_FILE,
    TUCKLY_ERRCO_MISSING_MAIN,
TUCKY_NUM_ERRCOS
} ErrorCode;

extern const char* strErrorCode[TUCKY_NUM_ERRCOS];

void cleanup();
#define safeExit(ERRCO, ...) {\
    printf("[ERRO] %s: ", strErrorCode[ERRCO]);\
    printf(__VA_ARGS__);\
    printf("\n");\
    cleanup(ERRCO);\
    exit(ERRCO);\
}

#endif /* TUCKY_ERROR_H */