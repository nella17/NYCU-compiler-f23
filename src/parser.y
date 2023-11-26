%code top {
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
}

%code requires {
#include "AST/AstDumper.hpp"
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

#include <vector>
#include <memory>

#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;
}

%code provides {
extern uint32_t line_num;   /* declared in scanner.l */
extern char current_line[]; /* declared in scanner.l */
extern FILE *yyin;          /* declared by lex */
extern char *yytext;        /* declared by lex */

static AstNode *root;

extern "C" int yylex(void);
void yyerror(const char *msg);
extern int yylex_destroy(void);
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *identifier;
    int integer;
    float real;
    char *string;

    AstNode *node;
    ProgramNode *program;
    CompoundStatementNode *compound_stmt;
};

    /* Delimiter */
%token COMMA SEMICOLON COLON LEFT_PARENTHESIS RIGHT_PARENTHESIS LEFT_SQUARE_BRACKETS RIGHT_SQUARE_BRACKETS

    /* Operator */
%token ADD MINUS MUL DIV MOD ASSIGN OP_LT OP_LTEQ OP_NEQ OP_GTEQ OP_GT OP_EQ AND OR NOT

%left AND OR
%right NOT
%left OP_LT OP_LTEQ OP_NEQ OP_GTEQ OP_GT OP_EQ
%left MINUS
%left ADD
%left DIV MOD
%left MUL
%right UMINUS

    /* Keyword */
%token KWvar KWarray KWof KWboolean KWinteger KWreal KWstring KWtrue KWfalse KWdef KWreturn KWbegin KWend KWwhile KWdo KWif KWthen KWelse KWfor KWto KWprint KWread

    /* Identifier */
%token <identifier> ID

    /* Literal */
%token <integer> INT_LITERAL
%token <real> REAL_LITERAL
%token <string> STRING_LITERAL

    /* Non-terminal */
%nterm <node> program_unit
%nterm <program> program
%nterm <compound_stmt> compound_statement

%%

root: program_unit {
    root = $1;
}

program_unit: program | function;

program:
    ID SEMICOLON
    declarations functions compound_statement
    KWend {
        $$ = new ProgramNode(
            @1.first_line, @1.first_column,
            $1, $5
        );
        free($1);
    }
;

functions: %empty | function functions
function: function_declaration | function_definition;
function_head: ID LEFT_PARENTHESIS arguments RIGHT_PARENTHESIS function_type;
function_type: %empty | COLON scalar_type;
function_declaration: function_head SEMICOLON;
function_definition: function_head compound_statement KWend;

arguments: %empty | arguments1
arguments1: argument | argument SEMICOLON arguments1
argument: identifier_list COLON type

declarations: %empty | declaration declarations
declaration:
    KWvar identifier_list COLON type SEMICOLON
    |
    KWvar identifier_list COLON literal_constant SEMICOLON
;

identifier_list: ID | ID COMMA identifier_list;

variable_reference: ID expr_brackets
expr_brackets: %empty | LEFT_SQUARE_BRACKETS expr RIGHT_SQUARE_BRACKETS expr_brackets

    /*
       Statements
                  */

statements: %empty | statement statements
statement: compound_statement | simple_statement | conditional_statement | while_statement | for_statement | return_statement | function_call

compound_statement:
    KWbegin
    declarations
    statements
    KWend {
        $$ = new CompoundStatementNode(@1.first_line, @1.first_column);
    }
;

simple_statement: assignment | print_statement | read_statement;
assignment: variable_reference ASSIGN expr SEMICOLON;
print_statement: KWprint expr SEMICOLON;
read_statement: KWread variable_reference SEMICOLON;

conditional_statement: KWif expr KWthen compound_statement KWelse compound_statement KWend KWif;
conditional_statement: KWif expr KWthen compound_statement KWend KWif;

while_statement: KWwhile expr KWdo compound_statement KWend KWdo;

for_statement: KWfor ID ASSIGN INT_LITERAL KWto INT_LITERAL KWdo compound_statement KWend KWdo;

return_statement: KWreturn expr SEMICOLON;

function_call: function_call_body SEMICOLON;
function_call_body: ID LEFT_PARENTHESIS expressions RIGHT_PARENTHESIS;

expressions: %empty | expressions1;
expressions1: expr | expr COMMA expressions1;
expr: literal_constant_pos | variable_reference | function_call_body | arith_expr
    | LEFT_PARENTHESIS expr RIGHT_PARENTHESIS;

arith_expr:
    MINUS expr %prec UMINUS
    |
    expr MUL expr
    |
    expr DIV expr
    |
    expr MOD expr
    |
    expr ADD expr
    |
    expr MINUS expr
    |
    expr OP_LT expr
    |
    expr OP_LTEQ expr
    |
    expr OP_NEQ expr
    |
    expr OP_GTEQ expr
    |
    expr OP_GT expr
    |
    expr OP_EQ expr
    |
    expr AND expr
    |
    expr OR expr
    |
    NOT expr
;

    /*
       Data Types
                                   */

type: array_type | scalar_type;
array_type: KWarray INT_LITERAL KWof type;
scalar_type: KWboolean | KWinteger | KWreal | KWstring;

literal_constant: neg_or_empty INT_LITERAL | neg_or_empty REAL_LITERAL | STRING_LITERAL | KWtrue | KWfalse;
literal_constant_pos: INT_LITERAL | REAL_LITERAL | STRING_LITERAL | KWtrue | KWfalse;
neg_or_empty: %empty | MINUS %prec UMINUS;

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
        AstDumper ast_dumper;
        root->accept(ast_dumper);
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
