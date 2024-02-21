#include "error.h"
#include "argparse.h"

#include <stdlib.h>

const char* strErrorCode[TUCKY_NUM_ERRCOS] = {
    "",
    "FileNotFound",
    "BadArgument",
    "MissingArgValue",

    "BadNumberFormat",
    "EmptyFile",

    "BadFile",
    "MissingMain"
};


void cleanup() {
    // extern FILE* SAFE_inputFile;
    extern FILE* SAFE_outputFile;
    // if (SAFE_inputFile  != NULL) fclose(SAFE_inputFile );
    if (SAFE_outputFile != NULL) fclose(SAFE_outputFile);

    extern TuckyArgParser* SAFE_parser;
    delArgParser(*SAFE_parser);

    /* Put stuff to be cleaned up here */
}