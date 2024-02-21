#ifndef TUCKY_LEXER_H
#define TUCKY_LEXER_H

#include "common.h"
#include "keywords.h"

typedef struct fileline_s {
    uint        line_num;
    char*       line;
    const char* origin;
    struct fileline_s* next;
} *FileLine;

typedef struct token_s {
    enum TokenType type;
    enum OperatorType op_type;
    uint     offset;
    FileLine origin;
    char*    text;
    struct   token_s* next;
} *Token;

FileLine newFileLine(const uint line_num, const char* file_path, char* line);
Token newToken(FileLine origin, const uint offset, const char* text);

void delFileLine(FileLine* head_ptr);
void delToken   (Token* head_ptr);

void appendToken(Token* head_ptr, Token token);

void dumpFileLines(FileLine head);
void dumpToken(Token head);
void dumpTokens(Token head);
void listTokens(Token head);

Token backToken(Token head);
#define frontToken(TOKENS) (TOKENS)
Token getToken(Token head, const enum TokenType type);

FileLine readFileAsLines(const char* file_path);
Token tokenizeFileLines(FileLine file_as_lines);

#define next(T)  (T->next)
#define next1(T) next(T)
#define next2(T) next1(next1(T))
#define next3(T) next2(next1(T))
#define next4(T) next2(next2(T))

#endif /* TUCKY_LEXER_H */