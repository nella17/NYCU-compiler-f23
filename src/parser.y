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
#include "type.hpp"

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

    AstNode *node_p;
    Asts *nodes_p;
    ProgramNode *program_p;
    DeclNodes *decls_p;
    DeclNode *decl_p;
    Functions *funcs_p;
    FunctionNode *func_p;
    IDs *ids_p;
    VariableReferenceNode *var_ref_p;
    CompoundStatementNode *compound_stmt_p;
    AssignmentNode *assign_p;
    PrintNode *print_stmt_p;
    ReadNode *read_stmt_p;
    IfNode *cond_stmt_p;
    WhileNode *while_stmt_p;
    ForNode *for_stmt_p;
    ReturnNode *ret_stmt_p;
    FunctionInvocationNode *func_call_p;
    Expressions *expres_p;
    ExpressionNode *expr_p;
    Type *type_p;
    ConstantValueNode *constant_p;
    bool neg;
};

    /* Delimiter */
%token COMMA SEMICOLON COLON LEFT_PARENTHESIS RIGHT_PARENTHESIS LEFT_SQUARE_BRACKETS RIGHT_SQUARE_BRACKETS

    /* Operator */
%token ADD MINUS MUL DIV MOD ASSIGN OP_LT OP_LTEQ OP_NEQ OP_GTEQ OP_GT OP_EQ AND OR NOT

%left AND
%left OR
%right NOT
%left OP_LT OP_LTEQ OP_NEQ OP_GTEQ OP_GT OP_EQ
%left ADD MINUS
%left MUL DIV MOD
%nonassoc UMINUS

    /* Keyword */
%token KWvar KWarray KWof KWboolean KWinteger KWreal KWstring KWtrue KWfalse KWdef KWreturn KWbegin KWend KWwhile KWdo KWif KWthen KWelse KWfor KWto KWprint KWread

    /* Identifier */
%token <identifier> ID

    /* Literal */
%token <integer> INT_LITERAL
%token <real> REAL_LITERAL
%token <string> STRING_LITERAL

    /* Non-terminal */
%nterm <node_p> program_unit statement
%nterm <nodes_p> statements
%nterm <program_p> program
%nterm <decls_p> declarations arguments arguments1
%nterm <decl_p> declaration argument
%nterm <funcs_p> functions
%nterm <func_p> function
%nterm <ids_p> identifier_list
%nterm <var_ref_p> variable_reference
%nterm <compound_stmt_p> compound_statement else_or_empty
%nterm <assign_p> assignment;
%nterm <print_stmt_p> print_statement
%nterm <read_stmt_p> read_statement
%nterm <cond_stmt_p> conditional_statement
%nterm <while_stmt_p> while_statement
%nterm <for_stmt_p> for_statement
%nterm <ret_stmt_p> return_statement
%nterm <func_call_p> function_call function_call_body
%nterm <expres_p> expressions expressions1 expr_brackets
%nterm <expr_p> expr
%nterm <type_p> type array_type scalar_type function_type
%nterm <constant_p> literal_constant literal_constant_pos string_or_bool
%nterm <neg> neg_or_empty

%%

root: program_unit {
    root = $1;
}

program_unit:
    program { $$ = $1; }
    |
    function { $$ = $1; }
;

program:
    ID SEMICOLON
    declarations functions compound_statement
    KWend {
        $$ = new ProgramNode(
            @1.first_line, @1.first_column,
            $1,
            $3, $4, $5
        );
    }
;

declarations:
    %empty {
        $$ = new DeclNodes;
    }
    |
    declarations declaration {
        $$ = $1;
        $$->emplace_back($2);
    }
;

declaration:
    KWvar identifier_list COLON type SEMICOLON {
        $$ = new DeclNode(
            @1.first_line, @1.first_column,
            $2, $4
        );
    }
    |
    KWvar identifier_list COLON literal_constant SEMICOLON {
        $$ = new DeclNode(
            @1.first_line, @1.first_column,
            $2, $4
        );
    }
;

functions:
    %empty {
        $$ = new Functions;
    }
    |
    functions function {
        $$ = $1;
        $$->emplace_back($2);
    }
;
function:
    ID LEFT_PARENTHESIS arguments RIGHT_PARENTHESIS function_type SEMICOLON {
        $$ = new FunctionNode(
            @1.first_line, @1.first_column,
            $1, $3, $5, nullptr
        );
    }
    |
    ID LEFT_PARENTHESIS arguments RIGHT_PARENTHESIS function_type compound_statement KWend {
        $$ = new FunctionNode(
            @1.first_line, @1.first_column,
            $1, $3, $5, $6
        );
    }
;
function_type:
    %empty {
        $$ = Type::makeVoid();
    }
    |
    COLON scalar_type {
        $$ = $2;
    }
;

arguments:
    %empty {
        $$ = new DeclNodes;
    }
    |
    arguments1
;
arguments1:
    argument {
        $$ = new DeclNodes;
        $$->emplace_back($1);
    }
    |
    arguments1 SEMICOLON argument {
        $$ = $1;
        $$->emplace_back($3);
    }
;
argument:
    identifier_list COLON type {
        $$ = new DeclNode(
            @1.first_line, @1.first_column,
            $1, $3
        );
    }
;

identifier_list:
    ID {
        $$ = new IDs;
        $$->emplace_back(@1.first_line, @1.first_column, $1);
    }
    |
    identifier_list COMMA ID {
        $$ = $1;
        $$->emplace_back(@3.first_line, @3.first_column, $3);
    }
;

variable_reference:
    ID expr_brackets {
        $$ = new VariableReferenceNode(
            @1.first_line, @1.first_column,
            $1, $2
        );
    }
;
expr_brackets:
    %empty {
        $$ = new Expressions;
    }
    |
    expr_brackets LEFT_SQUARE_BRACKETS expr RIGHT_SQUARE_BRACKETS {
        $$ = $1;
        $$->emplace_back($3);
    }
;

    /*
       Statements
                  */

statements:
    %empty {
        $$ = new Asts;
    }
    |
    statements statement {
        $$ = $1;
        $$->emplace_back($2);
    }
;
statement:
    compound_statement { $$ = $1; }
    |
    assignment { $$ = $1; }
    |
    print_statement { $$ = $1; }
    |
    read_statement { $$ = $1; }
    |
    conditional_statement { $$ = $1; }
    |
    while_statement { $$ = $1; }
    |
    for_statement { $$ = $1; }
    |
    return_statement { $$ = $1; }
    |
    function_call { $$ = $1; }

compound_statement:
    KWbegin
    declarations
    statements
    KWend {
        $$ = new CompoundStatementNode(
            @1.first_line, @1.first_column,
            $2, $3
        );
    }
;

assignment:
    variable_reference ASSIGN expr SEMICOLON {
        $$ = new AssignmentNode(
            @2.first_line, @2.first_column,
            $1, $3
        );
    }
;
print_statement:
    KWprint expr SEMICOLON {
        $$ = new PrintNode(
            @1.first_line, @1.first_column,
            $2
        );
    }
;
read_statement:
    KWread variable_reference SEMICOLON {
        $$ = new ReadNode(
            @1.first_line, @1.first_column,
            $2
        );
    }
;

conditional_statement:
    KWif expr KWthen
    compound_statement
    else_or_empty
    KWend KWif {
        $$ = new IfNode(
            @1.first_line, @1.first_column,
            $2, $4, $5
        );
    }
;

else_or_empty:
    %empty { $$ = nullptr; }
    |
    KWelse compound_statement {
        $$ = $2;
    }
;

while_statement:
    KWwhile expr KWdo
    compound_statement
    KWend KWdo {
        $$ = new WhileNode(
            @1.first_line, @1.first_column,
            $2, $4
        );
    }
;

for_statement:
    KWfor ID ASSIGN INT_LITERAL KWto INT_LITERAL KWdo
    compound_statement
    KWend KWdo {
        auto decl = new DeclNode(
            @2.first_line, @2.first_column,
            new IDs{ { @2.first_line, @2.first_column, $2 } },
            Type::makeInteger()
        );
        auto init = new AssignmentNode(
            @3.first_line, @3.first_column,
            new VariableReferenceNode(
                @2.first_line, @2.first_column,
                decl->getVars()[0],
                new Expressions
            ),
            new ConstantValueNode(
                @4.first_line, @4.first_column, $4
            )
        );
        auto end = new ConstantValueNode(
            @6.first_line, @6.first_column, $6
        );
        $$ = new ForNode(
            @1.first_line, @1.first_column,
            decl, init, end, $8
        );
    }
;

return_statement:
    KWreturn expr SEMICOLON {
        $$ = new ReturnNode(
            @1.first_line, @1.first_column,
            $2
        );
    }
;

function_call: function_call_body SEMICOLON { $$ = $1; };
function_call_body:
    ID LEFT_PARENTHESIS expressions RIGHT_PARENTHESIS {
        $$ = new FunctionInvocationNode(
            @1.first_line, @1.first_column,
            $1, $3
        );
    }
;

expressions:
    %empty { $$ = nullptr; }
    |
    expressions1
;
expressions1:
    expr {
        $$ = new Expressions;
        $$->emplace_back($1);
    }
    |
    expressions1 COMMA expr {
        $$ = $1;
        $$->emplace_back($3);
    }
;
expr:
    literal_constant_pos { $$ = $1; }
    |
    variable_reference { /* $$ = $1; */ }
    |
    function_call_body { $$ = $1; }
    |
    LEFT_PARENTHESIS expr RIGHT_PARENTHESIS { $$ = $2; }
    |
    MINUS expr %prec UMINUS {
        $$ = new UnaryOperatorNode(
			@1.first_line, @1.first_column,
			Operator::NEG, $2
		);
    }
    |
    expr MUL expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::MUL, $1, $3
		);
	}
    |
    expr DIV expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::DIV, $1, $3
		);
	}
    |
    expr MOD expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::MOD, $1, $3
		);
	}
    |
    expr ADD expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::ADD, $1, $3
		);
	}
    |
    expr MINUS expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::SUB, $1, $3
		);
	}
    |
    expr OP_LT expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::OP_LT, $1, $3
		);
	}
    |
    expr OP_LTEQ expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::OP_LTEQ, $1, $3
		);
	}
    |
    expr OP_NEQ expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::OP_NEQ, $1, $3
		);
	}
    |
    expr OP_GTEQ expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::OP_GTEQ, $1, $3
		);
	}
    |
    expr OP_GT expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::OP_GT, $1, $3
		);
	}
    |
    expr OP_EQ expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::OP_EQ, $1, $3
		);
	}
    |
    expr AND expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::AND, $1, $3
		);
	}
    |
    expr OR expr {
        $$ = new BinaryOperatorNode(
            @2.first_line, @2.first_column,
			Operator::OR, $1, $3
		);
	}
    |
    NOT expr {
        $$ = new UnaryOperatorNode(
			@1.first_line, @1.first_column,
			Operator::NOT, $2
		);
    }
;

    /*
       Data Types
                                   */

type: array_type | scalar_type;
array_type:
    KWarray INT_LITERAL KWof type {
        $$ = $4;
        $$->addDim($2);
    }
;
scalar_type:
    KWboolean { $$ = Type::makeBoolean(); }
    |
    KWinteger { $$ = Type::makeInteger(); }
    |
    KWreal    { $$ = Type::makeReal(); }
    |
    KWstring  { $$ = Type::makeString(); }
;

literal_constant:
    string_or_bool
    |
    neg_or_empty INT_LITERAL {
        if ($1) {
            $$ = new ConstantValueNode(
                @1.first_line, @1.first_column,
                -$2
            );
        } else {
            $$ = new ConstantValueNode(
                @2.first_line, @2.first_column,
                $2
            );
        }
    }
    |
    neg_or_empty REAL_LITERAL {
        if ($1) {
            $$ = new ConstantValueNode(
                @1.first_line, @1.first_column,
                -$2
            );
        } else {
            $$ = new ConstantValueNode(
                @2.first_line, @2.first_column,
                $2
            );
        }
    }
;
literal_constant_pos:
    string_or_bool
    |
    INT_LITERAL {
        $$ = new ConstantValueNode(
            @1.first_line, @1.first_column,
            $1
        );
    }
    |
    REAL_LITERAL {
        $$ = new ConstantValueNode(
            @1.first_line, @1.first_column,
            $1
        );
    }
;
string_or_bool:
    STRING_LITERAL {
        $$ = new ConstantValueNode(
            @1.first_line, @1.first_column,
            $1
        );
    }
    |
    KWtrue {
        $$ = new ConstantValueNode(
            @1.first_line, @1.first_column,
            true
        );
    }
    |
    KWfalse {
        $$ = new ConstantValueNode(
            @1.first_line, @1.first_column,
            false
        );
    }
;
neg_or_empty:
    %empty {
        $$ = false;
    }
    |
    MINUS %prec UMINUS {
        $$ = true;
    }
;

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

#ifdef TRACE
    yydebug = 1;
#endif

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
