#ifndef TUCKY_ARGS_H
#define TUCKY_ARGS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"

typedef struct tucky_arg_s {
    const char*  txt;
    struct tucky_arg_s* next;
} *TuckyArg;
#define TUCKY_NO_ARGS NULL

enum TuckyArgumentStatus {
    OPTIONAL=0,
    REQUIRED=1
};

typedef struct tucky_argument_s {
    enum TuckyArgumentStatus status;
    bool     enabled;
    char     flag;
    uint     nargs;
    const char*    keyword;
    const char*    help;
    TuckyArg args;
    struct tucky_argument_s* next;
} *TuckyArgument;

typedef struct {
    int     argc;
    char**  argv;
    char*   invoker;
    TuckyArgument args;
} TuckyArgParser;

/****************************************************************/

void addArgument(
    TuckyArgParser* parser,
    const char flag, const char* keyword, 
    const uint nargs, 
    const enum TuckyArgumentStatus status,
    const char* help);

void delArgParser(TuckyArgParser parser);

void printArguments(const TuckyArgParser parser);
void dumpArgs(const TuckyArgParser parser);

TuckyArgument getArgumentFromFlag   (TuckyArgParser parser, const char flag);
TuckyArgument getArgumentFromKeyword(TuckyArgParser parser, const char* keyword);

TuckyArgParser newArgParser(int argc, char** argv);
void parseArgs(TuckyArgParser parser);

#define STORE_TRUE 0
#define AS_MANY   -1
#define NARGS(N)   N

#endif /* TUCKY_ARGS_H */