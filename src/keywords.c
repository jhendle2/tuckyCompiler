#include "keywords.h"

#include <stdbool.h>
#include <string.h>

#include "common.h"
#include "error.h"
#include "utils.h"

const char* strTokenType[TUCKY_NUM_KEYWORDS] = {
    /* Pseudo-Keywords */
    "#invalid",
    "#operator",

    "#char_const",
    "#string_const",
    "#int_const",
    "#hex_const",
    "#float_const",
    "#identifier",
    "#namespace",
    
    /* Keywords */
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "inline",
    "long",
    "register",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while"
};

static inline bool isStringConst(const char* s) {
    const uint len = strlen(s);
    return (
        strlen(s) >= 2   &&
        s[0]     == '\"' &&
        s[len-1] == '\"'
    );
}

static inline bool isCharConst(const char* s) {
    const uint len = strlen(s);
    return (
        strlen(s) >= 2   &&
        s[0]     == '\'' &&
        s[len-1] == '\''
    );
}

static inline bool isIntConst(const char* s) {
    if (strlen(s) > 1 && s[0]=='-') s++;
    if (strlen(s) > 0) {
        const char* t = s;
        while (*t) {
            if (!isDecDigit(*t)) return false;
            t++;
        } return true;
    } return false;
}

static inline bool isFloatConst(const char* s) {
    if (strlen(s) > 1 && s[0]=='-') s++;
    if (strlen(s) > 0) {
        if (s[0] == '.') return false;
        const char* t = s;
        uint dot_count = 0;
        while (*t) {
            if (*t == '.') dot_count++;
            else if (!isDecDigit(*t)) return false;
            t++;
        }
        if (dot_count>1) safeExit(TUCKY_ERRCO_BAD_NUM_FORMAT, "A decimal may not contain more than 1 point");
        return true;
    } return false;
}

static inline bool isHexConst(const char* s) {
    if (strlen(s) > 2) {
        if (s[0] != '0' && s[1] != 'x') return false;

        const char* t = s+2; /* Skip the `0x` */
        while (*t) {
            if (!isHexDigit(*t)) return false;
            t++;
        } return true;
    } return false;
}

bool isIdentifier(const char* s) {
    if (strlen(s) == 0) return false;
    if (strlen(s) == 1) return (isAlpha(s[0]) || s[0]=='_');
    if (isAlpha(s[0]) || s[0]=='_') {
        const char* t = s;
        while (*t) {
            if (!(isAlphaNum(*t) || *t=='_')) return false;
            t++;
        }
    } return true;
}

enum TokenType getTokenType(const char* s) {
    if (isStringConst(s)) return TUCKY_STRING_CONST;
    if (isCharConst  (s)) return TUCKY_CHAR_CONST;
    if (isIntConst   (s)) return TUCKY_INT_CONST;
    if (isFloatConst (s)) return TUCKY_FLOAT_CONST;
    if (isHexConst   (s)) return TUCKY_HEX_CONST;
    if (isOperator   (s)) return TUCKY_OPERATOR;
    /* Slow so put at end of fast functions */
    for (uint i = (int)(TUCKY_AUTO); i<TUCKY_NUM_KEYWORDS; i++) {
        if (strcmp(s, strTokenType[i])==0) return (enum TokenType)(i);
    }
    if (isIdentifier (s)) return TUCKY_IDENTIFIER;
    return TUCKY_INVALID;
}

OpWeight getOpWeight(const enum OperatorType op) {
    switch (op) {
        default:
        case OP_INVALID : return 0;
        case OP_INCREMENT_POST : return 1;
        case OP_DECREMENT_POST : return 1;
        case OP_FN_CALL : return 1;
        case OP_INDEX : return 1;
        case OP_MEMBER : return 1;
        case OP_MEMBER_PTR : return 1;
        case OP_INCREMENT_PRE : return 2;
        case OP_DECREMENT_PRE : return 2;
        case OP_NOT_BOOLEAN : return 2;
        case OP_NOT_BITWISE : return 2;
        case OP_INDIRECTION : return 2;
        case OP_ADDRESS_OF : return 2;
        case OP_SIZEOF : return 2;
        case OP_MULTIPLY : return 3;
        case OP_DIVIDE : return 3;
        case OP_MODULO : return 3;
        case OP_ADDITION : return 4;
        case OP_SUBTRACTION : return 4;
        case OP_LEFT_SHIFT : return 5;
        case OP_RIGHT_SHIFT : return 5;
        case OP_LESS_THAN : return 6;
        case OP_LESS_EQUAL : return 6;
        case OP_GREATER_THAN : return 6;
        case OP_GREATER_EQUAL : return 6;
        case OP_EQUALITY : return 7;
        case OP_INEQUALITY : return 7;
        case OP_AND_BITWISE : return 8;
        case OP_XOR_BITWISE : return 9;
        case OP_OR_BITWISE : return 10;
        case OP_AND_BOOLEAN : return 11;
        case OP_OR_BOOLEAN : return 12;
        case OP_QUESTION : return 13;
        case OP_COLON : return 13;
        case OP_ASSIGN : return 14;
        case OP_ASSIGN_ADDITION : return 14;
        case OP_ASSIGN_SUBTRACTION : return 14;
        case OP_ASSIGN_MULTIPLY : return 14;
        case OP_ASSIGN_DIVIDE : return 14;
        case OP_ASSIGN_MODULO : return 14;
        case OP_ASSIGN_LEFT_SHIFT : return 14;
        case OP_ASSIGN_RIGHT_SHIFT : return 14;
        case OP_ASSIGN_AND_BITWISE : return 14;
        case OP_ASSIGN_XOR_BITWISE : return 14;
        case OP_ASSIGN_OR_BITWISE : return 14;
        case OP_COMMA : return 15;
    } return 0;
};

const char* strOpType[TUCKY_NUM_OPERATORS] = {
    "#nop",
    "++",
    "--",
    "#fn_call",
    "#index",
    ".",
    "->",
    "#increment_pre",
    "#decrement_pre",
    "!",
    "~",
    "#indirection",
    "&",
    "sizeof",
    "*",
    "/",
    "%%",
    "+",
    "-",
    "<<",
    ">>",
    "<",
    "<=",
    ">",
    ">=",
    "==",
    "!=",
    "&",
    "^",
    "|",
    "&&",
    "||",
    "?",
    ":",
    "=",
    "+=",
    "-=",
    "*=",
    "/=",
    "%%=",
    "<<=",
    ">>=",
    "&=",
    "^=",
    "|=",
    ",",
    ";",
    "#",
    "[",
    "]",
    "{",
    "}",
    "(",
    ")",
    "//",
    "/*",
    "*/"
};

enum OperatorType getOpType(const char* s) {
    /* Slow so put at end of fast functions */
    for (uint i = (int)(OP_INCREMENT_POST); i<TUCKY_NUM_OPERATORS; i++) {
        if (strcmp(s, strOpType[i])==0) return (enum OperatorType)(i);
    }
    return OP_INVALID;
}

bool isDelimiter(const char  c) {
    for (uint i = (int)(OP_INCREMENT_POST); i<TUCKY_NUM_OPERATORS; i++) {
        const char* op_str = strOpType[i];
        if (strlen(op_str)==1 && op_str[0]==c) return true;
    } return false;
}
bool isOperator (const char* s) {
    return getOpType(s) != OP_INVALID;
}