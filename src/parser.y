%{
#include "AST/BinaryOperator.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/ast.hpp"
#include "AST/decl.hpp"
#include "AST/expression.hpp"
#include "AST/for.hpp"
#include "AST/function.hpp"
#include "AST/if.hpp"
#include "AST/print.hpp"
#include "AST/program.hpp"
#include "AST/read.hpp"
#include "AST/return.hpp"
#include "AST/variable.hpp"
#include "AST/while.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

extern int32_t line_num;    /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

static AstNode *root;

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);
%}

%code requires {
    class AstNode;
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *identifier;

    AstNode *node;
};

%type <identifier> ProgramName ID

%token COMMA SEMICOLON COLON LEFT_PARENTHESIS RIGHT_PARENTHESIS LEFT_SQUARE_BRACKETS RIGHT_SQUARE_BRACKETS
%token OP_ADD OP_SUB OP_MUL OP_DIV OP_MOD OP_ASSIGN OP_LT OP_LTEQ OP_NEQ OP_GTEQ OP_GT OP_EQ OP_AND OP_OR OP_NOT
%token KWvar KWarray KWof KWboolean KWinteger KWreal KWstring KWtrue KWfalse KWdef KWreturn KWbegin KWend KWwhile KWdo KWif KWthen KWelse KWfor KWto KWprint KWread
%token ID INTEGER OCT_INTEGER FLOAT SCIENTIFIC STRING

%left OP_AND OP_OR OP_NOT
%left OP_LT OP_LTEQ OP_NEQ OP_GTEQ OP_GT OP_EQ
%left OP_SUB
%left OP_ADD
%left OP_DIV OP_MOD
%left OP_MUL
%nonassoc UMINUS

%%

ProgramName: ID { root = new ProgramNode(@1.first_line, @1.first_column, $1); }

program_unit: program | function;

program: identifier SEMICOLON vars_consts functions compound_statement KWend;

functions: /* empty */ | function functions
function: function_declaration | function_definition;
function_head: identifier LEFT_PARENTHESIS arguments RIGHT_PARENTHESIS function_type;
function_type: /* empty */ | COLON scalar_type;
function_declaration: function_head SEMICOLON;
function_definition: function_head compound_statement KWend;

arguments: /* empty */ | arguments1
arguments1: argument | argument SEMICOLON arguments1
argument: identifier_list COLON type

vars_consts: /* empty */ | var_const vars_consts
var_const: variable | constant

variable: KWvar identifier_list COLON type SEMICOLON;

variable_reference: identifier expr_brackets
expr_brackets: /* empty */ | LEFT_SQUARE_BRACKETS expr RIGHT_SQUARE_BRACKETS expr_brackets

constant: KWvar identifier_list COLON literal_constant SEMICOLON;
literal_constant: integer_constant | FLOAT | SCIENTIFIC | STRING | KWtrue | KWfalse;
integer_constant: INTEGER | OCT_INTEGER;

statements: /* empty */ | statement statements
statement: compound_statement | simple_statement | conditional_statement | while_statement | for_statement | return_statement | function_call

compound_statement: KWbegin vars_consts statements KWend;

simple_statement: assignment | print_statement | read_statement;
assignment: variable_reference OP_ASSIGN expr SEMICOLON;
print_statement: KWprint expr SEMICOLON;
read_statement: KWread variable_reference SEMICOLON;

conditional_statement: KWif expr KWthen compound_statement KWelse compound_statement KWend KWif;
conditional_statement: KWif expr KWthen compound_statement KWend KWif;

while_statement: KWwhile expr KWdo compound_statement KWend KWdo;

for_statement: KWfor identifier OP_ASSIGN integer_constant KWto integer_constant KWdo compound_statement KWend KWdo;

return_statement: KWreturn expr SEMICOLON;

function_call: function_call_body SEMICOLON;
function_call_body: identifier LEFT_PARENTHESIS expressions RIGHT_PARENTHESIS;

expressions: /* empty */ | expressions1;
expressions1: expr | expr COMMA expressions1;
expr: literal_constant | variable_reference | function_call_body | arith_expr
    | LEFT_PARENTHESIS expr RIGHT_PARENTHESIS;

arith_expr: OP_SUB expr %prec UMINUS;
arith_expr: expr OP_MUL expr;
arith_expr: expr OP_DIV expr;
arith_expr: expr OP_MOD expr;
arith_expr: expr OP_ADD expr;
arith_expr: expr OP_SUB expr;
arith_expr: expr OP_LT expr;
arith_expr: expr OP_LTEQ expr;
arith_expr: expr OP_NEQ expr;
arith_expr: expr OP_GTEQ expr;
arith_expr: expr OP_GT expr;
arith_expr: expr OP_EQ expr;
arith_expr: expr OP_AND expr;
arith_expr: expr OP_OR expr;
arith_expr: OP_NOT expr;

identifier: ID;
identifier_list: identifier | identifier COMMA identifier_list;

type: array_type | scalar_type;
array_type: KWarray integer_constant KWof type;
scalar_type: KWboolean | KWinteger | KWreal | KWstring;

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
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename> [--dump-ast]\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("fopen() failed");
        exit(-1);
    }

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        root->print();
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
