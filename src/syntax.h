#ifndef TUCKY_SYNTAX_H
#define TUCKY_SYNTAX_H

#include "lexer.h"

enum NodeType {
    NT_INVALID=0,
    NT_FUNCTION,
    NT_CONDITIONAL,
    NT_NAMESPACE,
    NT_STATEMENT,
    NT_CLOSE_SCOPE,
NUM_NODE_TYPES
};
extern const char* strNodeType[];

typedef struct syntaxnode_s {
    enum NodeType type;
    Token  tokens;
    struct syntaxnode_s* parent;
    struct syntaxnode_s* next;
    struct syntaxnode_s* children;
} *SyntaxNode;

void delSyntaxNode(SyntaxNode* head_ptr);

void treeSyntaxNode(SyntaxNode snode, const uint level);
SyntaxNode buildSyntaxTree(Token tokens);

SyntaxNode getChildFromIdentifier(SyntaxNode snode, const char* identifier);

#endif /* TUCKY_SYNTAX_H */