#ifndef TUCKY_KEYWORDS_H
#define TUCKY_KEYWORDS_H

#include <stdbool.h>

enum TokenType {
    /* Pseudo-Keywords */
    TUCKY_INVALID=0,
    TUCKY_OPERATOR,

    TUCKY_CHAR_CONST,
    TUCKY_STRING_CONST,
    TUCKY_INT_CONST,
    TUCKY_HEX_CONST,
    TUCKY_FLOAT_CONST,
    TUCKY_IDENTIFIER,
    TUCKY_NAMESPACE,

    /* Keywords */
    TUCKY_AUTO,
    TUCKY_BREAK,
    TUCKY_CASE,
    TUCKY_CHAR,
    TUCKY_CONST,
    TUCKY_CONTINUE,
    TUCKY_DEFAULT,
    TUCKY_DO,
    TUCKY_DOUBLE,
    TUCKY_ELSE,
    TUCKY_ENUM,
    TUCKY_EXTERN,
    TUCKY_FLOAT,
    TUCKY_FOR,
    TUCKY_GOTO,
    TUCKY_IF,
    TUCKY_INT,
    TUCKY_INLINE,
    TUCKY_LONG,
    TUCKY_REGISTER,
    TUCKY_RETURN,
    TUCKY_SHORT,
    TUCKY_SIGNED,
    TUCKY_SIZEOF,
    TUCKY_STATIC,
    TUCKY_STRUCT,
    TUCKY_SWITCH,
    TUCKY_UNION,
    TUCKY_UNSIGNED,
    TUCKY_VOID,
    TUCKY_VOLATILE,
    TUCKY_WHILE,
TUCKY_NUM_KEYWORDS,
};

enum OperatorType {
    OP_INVALID=0,
    OP_INCREMENT_POST,
    OP_DECREMENT_POST,
    OP_FN_CALL,
    OP_INDEX,
    OP_MEMBER,
    OP_MEMBER_PTR,
    OP_INCREMENT_PRE,
    OP_DECREMENT_PRE,
    OP_NOT_BOOLEAN,
    OP_NOT_BITWISE,
    OP_INDIRECTION,
    OP_ADDRESS_OF,
    OP_SIZEOF,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MODULO,
    OP_ADDITION,
    OP_SUBTRACTION,
    OP_LEFT_SHIFT,
    OP_RIGHT_SHIFT,
    OP_LESS_THAN,
    OP_LESS_EQUAL,
    OP_GREATER_THAN,
    OP_GREATER_EQUAL,
    OP_EQUALITY,
    OP_INEQUALITY,
    OP_AND_BITWISE,
    OP_XOR_BITWISE,
    OP_OR_BITWISE,
    OP_AND_BOOLEAN,
    OP_OR_BOOLEAN,
    OP_QUESTION,
    OP_COLON,
    OP_ASSIGN,
    OP_ASSIGN_ADDITION,
    OP_ASSIGN_SUBTRACTION,
    OP_ASSIGN_MULTIPLY,
    OP_ASSIGN_DIVIDE,
    OP_ASSIGN_MODULO,
    OP_ASSIGN_LEFT_SHIFT,
    OP_ASSIGN_RIGHT_SHIFT,
    OP_ASSIGN_AND_BITWISE,
    OP_ASSIGN_XOR_BITWISE,
    OP_ASSIGN_OR_BITWISE,

    OP_COMMA,
    OP_SEMICOLON,
    OP_HASH,
    OP_BRACKET_OPEN,
    OP_BRACKET_CLOSE,
    OP_BRACE_OPEN,
    OP_BRACE_CLOSE,
    OP_PAREN_OPEN,
    OP_PAREN_CLOSE,

    OP_COMMENT_LINE,
    OP_COMMENT_BLOCK_OPEN,
    OP_COMMENT_BLOCK_CLOSE,
TUCKY_NUM_OPERATORS
};

extern const char* strTokenType[TUCKY_NUM_KEYWORDS];
extern const char* strOpType   [TUCKY_NUM_OPERATORS];

enum TokenType getTokenType(const char* s);
enum OperatorType getOpType(const char* s);

typedef unsigned int OpWeight;
OpWeight getOpWeight(const enum OperatorType op);

bool isDelimiter(const char  c);
bool isOperator (const char* s);

#endif /* TUCKY_KEYWORDS_H */