#include "qbe.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "error.h"

static
bool verifyIdentifierExists(SyntaxNode scope, const char* identifier) {
    return (getChildFromIdentifier(scope, identifier)!=NULL);
}

typedef char StringBuffer[512];

static
void compileSyntaxNodeHelper(FILE* fp, SyntaxNode snode, StringBuffer data_segment) {
    static uint const_counter = 0;
    StringBuffer local_data_segment = {0};
    if (snode->type == NT_FUNCTION) {
        const Token fn_identifier = getToken(snode->tokens, TUCKY_IDENTIFIER);
        assert(fn_identifier);
        const char* fn_name = fn_identifier->text;
        fprintf(fp, "%sfunction w $%s() {\n",
            (strcmp(fn_name,"main")==0)?"export ":"",
            fn_name
        );
        fprintf(fp, "@start\n");
    }
    if (snode->type == NT_STATEMENT) {
        if (snode->tokens->type == TUCKY_RETURN) {
            const Token ret_value = snode->tokens->next;
            fprintf(fp, "\tret %s\n", ret_value->text);
        }
        if (snode->tokens->type == TUCKY_IDENTIFIER) { /* Function call */ // TODO: Run expression builder first!!
            const char* fn_name = snode->tokens->text;
            const char* fmt     = next2(snode->tokens)->text;
            fprintf(fp, "\tcall $%s(l $string_const_%u, ...)\n", fn_name, const_counter);
            sprintf(local_data_segment, "data $string_const_%u = { b %s, b 0 }\n", const_counter, fmt);
            const_counter++;
            strcat(data_segment, local_data_segment);
        }
    }
    if (snode->type == NT_CLOSE_SCOPE) {
        fprintf(fp, "}\n");
    }
}

static
void compileSyntaxNode(FILE* fp, SyntaxNode snode, StringBuffer data_segment) {
    compileSyntaxNodeHelper(fp, snode, data_segment);
    for (SyntaxNode child = snode->children; child!=NULL; child=child->next) {
        compileSyntaxNode(fp, child, data_segment);
    }
}

FILE* SAFE_outputFile = NULL;
void compileToQBE(const char* output_path, const SyntaxNode master_node) {
    FILE* fp = fopen(output_path, "w"); SAFE_outputFile = fp;
    StringBuffer data_segment = {0};

    if (fp == NULL) {
        safeExit(TUCKLY_ERRCO_BAD_FILE, "Cannot open file path `%s` for writing.", output_path);
    }

    const bool has_main = verifyIdentifierExists(master_node, "main");
    if (!has_main) {
        safeExit(TUCKLY_ERRCO_MISSING_MAIN, "No `main` method was defined in this namespace.");
    }

    fprintf(fp, "## Compiled from [%s]\n\n", master_node->tokens->text);
    compileSyntaxNode(fp, master_node->children, data_segment);
    fprintf(fp, "\n%s", data_segment);
    // fputs("export function w $main() {""\n", fp);
    // fputs("@start"                     "\n", fp);
    // fputs("\tret 0"                    "\n", fp);
    // fputs("}"                          "\n", fp);
// END:
    fclose(fp);
}