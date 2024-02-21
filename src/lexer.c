#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "utils.h"
#include "keywords.h"

/****************************************************************/
FileLine newFileLine(const uint line_num, const char* file_path, char* line) {
    FileLine fl  = (FileLine)malloc(sizeof(struct fileline_s));
    fl->origin   = file_path;
    fl->line_num = line_num;
    fl->next     = NULL;

    const uint len = strlen(line);
    fl->line = (char*)malloc(sizeof(char)*(len+1));
    strcpy(fl->line, line);
    if (len > 0 && fl->line[len-1] == '\n') fl->line[len-1]=0;
    return fl;
}

static
void appendFileLine(FileLine* head_ptr, FileLine fl) {
    if (head_ptr==NULL || (*head_ptr)==NULL) {
        (*head_ptr) = fl;
        return;
    }

    if ( (*head_ptr)->next == NULL) {
        (*head_ptr)->next = fl;
        return;
    }

    appendFileLine( &((*head_ptr)->next), fl);
}

void delFileLine(FileLine* head_ptr) {
    if (head_ptr==NULL || *head_ptr==NULL) return;
    delFileLine(&((*head_ptr)->next));
    if ((*head_ptr)->line) free((*head_ptr)->line);
    free(*(head_ptr));
    (*head_ptr) = NULL;
}

void dumpFileLines(FileLine head) {
    if (head == NULL) return;
    printf("%s:%u: %s\n", head->origin, head->line_num, head->line);
    if (head->next) dumpFileLines(head->next);
}

static
void stripComments(bool* opened_a_comment, char buf[256]) {
    char* t = buf;
    while (*t) {
        if (*t=='*' && *(t+1)=='/') {
            if (*opened_a_comment) {
                *opened_a_comment = false;
                char temp[256] = "";
                strncpy(temp, t+2, 255);
                strncat(buf, temp, 255);
                goto NEXT;
            }
        }
        if (*opened_a_comment) {
            *t=0;
            goto NEXT;
        }

        if (*t=='/' && *(t+1)=='/') {
            *t=0;
            break;
        }

        if (*t=='/' && *(t+1)=='*') {
            *opened_a_comment = true;
            *t=0;
            goto NEXT;
        }
NEXT:
        t++;
    }
}

FILE* SAFE_inputFile = NULL;
FileLine readFileAsLines(const char* file_path) {
    FILE* fp = fopen(file_path, "r"); SAFE_inputFile = fp;
    if (fp == NULL) {
        safeExit(TUCKY_ERRCO_FILE_NOT_FOUND, "No such file `%s`", file_path);
    }

    FileLine file_as_lines = NULL;

    char buf[256] = "";
    int line_num = 0;
    bool opened_a_comment = false;
    while (fgets(buf, 256, fp) != NULL) {
        line_num++;
        if (buf[0] == '#') continue;

        stripComments(&opened_a_comment, buf);
        if (strlen(buf)>0) {
            FileLine fl = newFileLine(line_num, file_path, buf);
            appendFileLine(&file_as_lines, fl);
        }
    }

    fclose(fp);
    return file_as_lines;
}

/****************************************************************/
Token newToken(FileLine origin, const uint offset, const char* text) {
    Token token    = (Token)malloc(sizeof(struct token_s));
    token->type    = getTokenType(text);
    token->op_type = (token->type==TUCKY_OPERATOR) ? getOpType(text): OP_INVALID;
    token->offset  = offset;
    token->origin  = origin;
    token->next    = NULL;
    token->text    = (char*)malloc(sizeof(char)*(strlen(text)+1));
    strcpy(token->text, text);
    return token;
}

void delToken(Token* head_ptr) {
    if (head_ptr==NULL || *head_ptr==NULL) return;
    delToken(&((*head_ptr)->next));
    if ((*head_ptr)->text) free((*head_ptr)->text);
    free(*(head_ptr));
    (*head_ptr) = NULL;
}

void dumpToken(Token head) {
    if (head == NULL) return;
    if (head->type == TUCKY_INVALID) {
        printf("%s:%3u:%3u: %15s : %s\n",
            head->origin->origin,
            head->origin->line_num, head->offset,
            "", head->text
        );
    } else if (head->type == TUCKY_OPERATOR) { 
        printf("%s:%3u:%3u: %15s : %s\n",
            head->origin->origin,
            head->origin->line_num, head->offset,
            strOpType[getOpType(head->text)], head->text
        );
    } else {
        printf("%s:%3u:%3u: %15s : %s\n",
            head->origin->origin,
            head->origin->line_num, head->offset,
            strTokenType[head->type], head->text
        );
    }
}

void dumpTokens(Token head) {
    dumpToken(head);
    if (head->next) dumpTokens(head->next);
}

void listTokens(Token head) {
    if (head == NULL) {
        printf("[]");
        return;
    }
    if (head->type == TUCKY_NAMESPACE)
         printf("[[%s]] ", head->text);
    else printf(  "%s "  , head->text);
    
    if (head->next) listTokens(head->next);
}

Token backToken(Token head) {
    if (head) {
        if (head->next) return backToken(head->next);
        return head;
    } return NULL;
}

Token getToken(Token head, const enum TokenType type) {
    if (head == NULL)       return NULL;
    if (head->type == type) return head;
    if (head->next != NULL) return getToken(head->next, type);
    return NULL;
}

void appendToken(Token* head_ptr, Token token) {
    if (head_ptr==NULL || (*head_ptr)==NULL) {
        (*head_ptr) = token;
        return;
    }

    if ( (*head_ptr)->next == NULL) {
        (*head_ptr)->next = token;
        return;
    }

    appendToken( &((*head_ptr)->next), token);
}

static
Token tokenizeFileLine(FileLine fl) {
    Token line_as_tokens = NULL;

    char* t = fl->line;
    uint buf_len    = 0;
    char buffer[64] = "";

    #define pushBuffer() {\
        if (buf_len > 0) {\
            buffer[buf_len]=0;\
            Token temp = newToken(fl, t-fl->line-buf_len+1, buffer);\
            appendToken(&line_as_tokens, temp);\
            buf_len = 0; buffer[0]=0;\
        }\
    }

    #define pushChar() buffer[buf_len++] = *t

    bool in_string = false;
    bool in_char   = false;

    while (*t) {
        if (!in_char && *t == '\"') {
            if (in_string) {
                pushChar();
                pushBuffer();
            } else {
                pushBuffer();
                pushChar();
            }
            in_string = !in_string;
            goto NEXT;
        }
        if (!in_string && *t == '\'') {
            if (in_char) {
                pushChar();
                pushBuffer();
            } else {
                pushBuffer();
                pushChar();
            }
            in_char = !in_char;
            goto NEXT;
        }
        if (in_string || in_char) {
            pushChar();
            goto NEXT;
        }
        

        if (isSpace(*t)) {
            pushBuffer();
            goto NEXT;
        }

        if (*t == '.' && isDecDigit(buffer[buf_len-1])) {
            pushChar();
            goto NEXT;
        }

        if (buf_len>0 && isDelimiter(buffer[buf_len-1])) {
            if (isDelimiter(*t)) {
                char temp_op[4] = "";
                strncat(temp_op, buffer, buf_len);
                strncat(temp_op, t     , 1);
                if (isOperator(temp_op)) {
                    pushChar();
                    goto NEXT;
                }
            }
            if (buffer[buf_len-1]=='.' && isDecDigit(*t)) {
                pushChar();
                goto NEXT;
            }
            pushBuffer();
            pushChar();
            goto NEXT;
        }

        if (isDelimiter(*t)) {
            pushBuffer();
            pushChar();
            goto NEXT;
        }

        pushChar();
    NEXT:
        t++;
    }
    pushBuffer();

    return line_as_tokens;
}

Token tokenizeFileLines(FileLine file_as_lines) {
    Token file_as_tokens = NULL;

    FileLine fl = file_as_lines;
    while (fl) {
        Token line_as_tokens = tokenizeFileLine(fl);
        appendToken(&file_as_tokens, line_as_tokens);
        fl = fl->next;
    }

    return file_as_tokens;
}

/****************************************************************/