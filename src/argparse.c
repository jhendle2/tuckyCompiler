#include "argparse.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>

static TuckyArg newArg(const char* txt) {
    TuckyArg arg = (TuckyArg)malloc(sizeof(struct tucky_arg_s));
    arg->txt  = txt;
    arg->next = NULL;
    return arg;
}

static void appendArg(TuckyArg* head_ptr, TuckyArg arg) {
    if (head_ptr==NULL || (*head_ptr)==NULL) {
        (*head_ptr) = arg;
        return;
    }

    if ( (*head_ptr)->next == NULL) {
        (*head_ptr)->next = arg;
        return;
    }

    appendArg( &((*head_ptr)->next), arg);
}

static void printArg(const TuckyArg arg) {
    if (arg) {
        printf("(%s) ", arg->txt);
        printArg(arg->next);
    }
}

static void delArg(TuckyArg* arg_ptr) {
    if (arg_ptr==NULL || *arg_ptr==NULL) return;
    if ((*arg_ptr)->next) delArg(&((*arg_ptr)->next));
    free(*arg_ptr);
    (*arg_ptr) = NULL;
}

static TuckyArgument newArgument(
    const char flag, const char* keyword, 
    const uint nargs,
    const enum TuckyArgumentStatus status,
    const char* help) {
        
    TuckyArgument argument = (TuckyArgument)malloc(sizeof(struct tucky_argument_s));
    argument->flag    = flag;
    argument->nargs   = nargs;
    argument->keyword = keyword;
    argument->help    = help;
    argument->status  = status;
    argument->enabled = false;
    argument->args    = TUCKY_NO_ARGS;
    argument->next    = NULL;

    return argument;
}

static void appendTuckyArgument(TuckyArgument* head_ptr, TuckyArgument argument) {
    if (head_ptr==NULL || (*head_ptr)==NULL) {
        (*head_ptr) = argument;
        return;
    }

    if ( (*head_ptr)->next == NULL) {
        (*head_ptr)->next = argument;
        return;
    }

    appendTuckyArgument( &((*head_ptr)->next), argument);
}

void addArgument(
    TuckyArgParser* parser,
    const char flag, const char* keyword, 
    const uint nargs, 
    const enum TuckyArgumentStatus status,
    const char* help) {

    TuckyArgument argument = newArgument(flag, keyword, nargs, status, help);
    appendTuckyArgument(&(parser->args), argument);

    return;
}

void delArgument(TuckyArgument* arg_ptr) {
    if (arg_ptr==NULL || *arg_ptr==NULL) return;
    delArgument(&((*arg_ptr)->next));
    delArg(&((*arg_ptr)->args));
    free(*(arg_ptr));
    (*arg_ptr) = NULL;
}

void delArgParser(TuckyArgParser parser) {
    delArgument(&(parser.args));
}

void printArguments(const TuckyArgParser parser) {
    printf("==== %s ====\n", parser.invoker);
    TuckyArgument temp = parser.args;
    while (temp) {
        printf("  -%c --%-10s [%u] : %s\n",
            temp->flag, temp->keyword,
            temp->nargs, temp->help
        );
        temp = temp->next;
    }
}
void dumpArgs(const TuckyArgParser parser) {
    printf("==== DUMP %s ====\n", parser.invoker);
    TuckyArgument temp = parser.args;
    while (temp) {
        printf("%10s : ", temp->keyword);
        if (temp->nargs > 0) printArg(temp->args);
        else printf("%s", temp->enabled?"ENABLED":"DISABLED");
        printf("\n");
        temp = temp->next;
    }
}

TuckyArgument getArgumentFromFlag   (TuckyArgParser parser, const char flag) {
    TuckyArgument argument = parser.args;
    while (argument) {
        if (argument->flag==flag) return argument;
        argument = argument->next;
    }
    return NULL;
}

TuckyArgument getArgumentFromKeyword(TuckyArgParser parser, const char* keyword) {
    TuckyArgument argument = parser.args;
    while (argument) {
        if (strcmp(argument->keyword, keyword)==0) return argument;
        argument = argument->next;
    }
    return NULL;
}

TuckyArgParser newArgParser(int argc, char** argv) {
    TuckyArgParser parser = {
        .argc=argc,
        .argv=argv,
        .invoker=argv[0],
        .args=TUCKY_NO_ARGS
    };
    addArgument(&parser, 'h', "help"   , 0, OPTIONAL, "Display the help information");
    return parser;
}

void parseArgs(TuckyArgParser parser) {
    TuckyArgument last_arg   = NULL;
    TuckyArg      string_arg = NULL;

    #define pushToArgument() {\
        if (last_arg) {\
            if ((last_arg->nargs==AS_MANY || last_arg->nargs>0) && string_arg) {\
                appendArg(&(last_arg->args), string_arg);\
                string_arg = NULL;\
            } else {\
                last_arg->enabled = true;\
            }\
        }\
    }

    for (int i = 1; i<parser.argc; i++) { /* Start at 1 to skip executable name */
        const char* arg = parser.argv[i];

        if (arg[0] == '-') {
            pushToArgument();
            if (arg[1] == '-') {
                /* Keyword */
                last_arg = getArgumentFromKeyword(parser, arg+2);
                if (last_arg == NULL)
                    safeExit(TUCKY_ERRCO_BAD_ARGUMENT, "`--%s`", arg+2);
                continue;
            }

            /* Flag */
            last_arg = getArgumentFromFlag(parser, arg[1]);
            if (last_arg == NULL)
                safeExit(TUCKY_ERRCO_BAD_ARGUMENT, "`-%c`", arg[1]);
            continue;
        }

        if (last_arg != NULL) {
            TuckyArg temp_string_arg = newArg(arg);
            appendArg(&string_arg, temp_string_arg);
        }
    }
    pushToArgument();

    TuckyArgument temp = parser.args;
    while (temp) {
        if (
            temp->status == REQUIRED &&
            temp->nargs  >  0        &&
            temp->args   == NULL
        ) {
            safeExit(TUCKY_ERRCO_MISSING_ARG, "Missing value for argument `%s`", temp->keyword);
        }
        temp = temp->next;
    }
}
