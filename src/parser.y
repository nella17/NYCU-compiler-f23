%{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern int32_t line_num;    /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

extern int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

%token COMMA SEMICOLON COLON LEFT_PARENTHESIS RIGHT_PARENTHESIS LEFT_SQUARE_BRACKETS RIGHT_SQUARE_BRACKETS
%token OP_ADD OP_SUB OP_MUL OP_DIV OP_MOD OP_ASSIGN OP_LT OP_LTEQ OP_NEQ OP_GTEQ OP_GT OP_EQ OP_AND OP_OR OP_NOT
%token KWvar KWarray KWof KWboolean KWinteger KWreal KWstring KWtrue KWfalse KWdef KWreturn KWbegin KWend KWwhile KWdo KWif KWthen KWelse KWfor KWto KWprint KWread
%token ID
%token INTEGER OCT_INTEGER FLOAT SCIENTIFIC STRING

%%

ProgramName: ID

%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, current_line, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }

    yyparse();

    fclose(yyin);
    yylex_destroy();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
