#include "syntax.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* strNodeType[] = {
    "Invalid",
    "Function",
    "Conditional",
    "Namespace",
    "Statement",
    "CloseScope",
};

static
SyntaxNode newSyntaxNode() {
    SyntaxNode snode = (SyntaxNode)malloc(sizeof(struct syntaxnode_s));
    snode->type     = NT_INVALID;
    snode->tokens   = NULL;
    snode->parent   = NULL;
    snode->next     = NULL;
    snode->children = NULL;
    return snode;
}

void treeSyntaxNode(SyntaxNode snode, const uint level) {
    if (snode == NULL) return;
    for (uint i = 0; i<level; i++) printf(" * ");
    printf("(%s) ", strNodeType[snode->type]);
    listTokens(snode->tokens); printf("\n");
    if (snode->children) treeSyntaxNode(snode->children, level+1);
    if (snode->next)     treeSyntaxNode(snode->next, level);
}

void delSyntaxNode(SyntaxNode* head_ptr) {
    if (head_ptr==NULL || *head_ptr==NULL) return;

    delToken( &((*head_ptr)->tokens) );

    if ( (*head_ptr)->children) delSyntaxNode( &((*head_ptr)->children) );
    if ( (*head_ptr)->next)     delSyntaxNode( &((*head_ptr)->next) );

    free(*head_ptr);
    (*head_ptr) = NULL;
}

static
void appendSyntaxNode(SyntaxNode* head_ptr, SyntaxNode snode) {
    if (head_ptr==NULL || (*head_ptr)==NULL) {
        (*head_ptr) = snode;
        return;
    }

    if ( (*head_ptr)->next == NULL) {
        (*head_ptr)->next = snode;
        return;
    }

    appendSyntaxNode( &((*head_ptr)->next), snode);
}

static
void addChildNode(SyntaxNode parent, SyntaxNode child) {
    if (parent == NULL) return;
    child->parent = parent;
    appendSyntaxNode(&(parent->children), child);
}

static inline bool isOpenScope(const Token token) {
    return (
        token != NULL &&
        token->op_type == OP_BRACE_OPEN
    );
}
static inline bool isCloseScope(const Token token) {
    return (
        token != NULL &&
        token->op_type == OP_BRACE_CLOSE
    );
}
static inline bool isStatement(const Token token) {
    return (
        token != NULL &&
        token->op_type == OP_SEMICOLON
    );
}

enum NodeType deduceNodeType(SyntaxNode snode) {
    Token front = frontToken(snode->tokens);
    Token back  = backToken (snode->tokens);
    if (front==NULL || back==NULL) return NT_INVALID;

    if (back->op_type == OP_SEMICOLON  ) {
        return (snode->type = NT_STATEMENT);
    }
    if (back->op_type == OP_BRACE_OPEN ) {
        if (
            front->type == TUCKY_IF   ||
            front->type == TUCKY_ELSE ||
            front->type == TUCKY_SWITCH
        )   return (snode->type = NT_CONDITIONAL);
        return (snode->type = NT_FUNCTION);
    }
    if (back->op_type == OP_BRACE_CLOSE) {
        return (snode->type = NT_CLOSE_SCOPE);
    }
    return NT_INVALID;
}

SyntaxNode buildSyntaxTree(Token tokens) {
    SyntaxNode master_node  = newSyntaxNode();
    appendToken(&(master_node->tokens), newToken(tokens->origin, 0, tokens->origin->origin));
    master_node->tokens->type = TUCKY_NAMESPACE;
    master_node->type         = NT_NAMESPACE;
    SyntaxNode current_node   = master_node;

    Token token_buffer = NULL;

    #define pluckToken(TOKEN, NEXT) {\
        if (TOKEN != NULL) {\
            NEXT = TOKEN->next;\
            TOKEN->next = NULL;\
        }\
    }

    #define createAndPushChild(NODE_NAME) \
        SyntaxNode NODE_NAME = newSyntaxNode();\
        NODE_NAME->tokens = token_buffer;\
        deduceNodeType(NODE_NAME);\
        addChildNode(current_node, NODE_NAME);\
        token_buffer = NULL;

    Token temp = tokens, next=NULL;
    while (temp) {
        pluckToken(temp, next);
        // listTokens(token_buffer); printf("\n");

        Token back = backToken(token_buffer);
        if (isOpenScope(back)) {
            createAndPushChild(child_node);
            current_node = child_node;
        }
        if (isStatement(back)) {
            createAndPushChild(child_node);
        }
        if (isCloseScope(back)) {
            createAndPushChild(child_node);
            current_node = current_node->parent;
        }
        // dumpToken(temp);

        appendToken(&token_buffer, temp);
        temp = next;
    };

    appendToken(&token_buffer, temp);
    createAndPushChild(child_node);

    return master_node;
}

SyntaxNode getChildFromIdentifier(SyntaxNode snode, const char* identifier) {
    if (snode->type == NT_FUNCTION) {
        Token fn_name = snode->tokens;
        while (fn_name != NULL) {
            if (strcmp(fn_name->text, identifier)==0) return snode;
            fn_name = fn_name->next;
        }
    }
    if (snode->type == NT_STATEMENT) { // FIXME: Should only return for assignments
        Token decl_name = snode->tokens;
        while (decl_name != NULL) {
            if (strcmp(decl_name->text, identifier)==0) return snode; // FIXME: Confirm there's an `=` after the identifier?
            decl_name = decl_name->next;
        }
    }
    if (snode->children) {
        SyntaxNode child = snode->children;
        while (child) {
            SyntaxNode match = getChildFromIdentifier(child, identifier);
            if (match != NULL) return match;

            child = child->next;
        }
    } return NULL;
}