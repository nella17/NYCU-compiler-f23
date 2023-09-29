%{
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define LIST_SOURCE                 appendToCurrentLine(yytext)
#define LIST_TOKEN(name)            do { LIST_SOURCE; if(opt_tok) printf("<%s>\n", name); } while(0)
#define LIST_LITERAL(name, literal) do { LIST_SOURCE; if(opt_tok) printf("<%s: %s>\n", name, literal); } while(0)
#define MAX_LINE_LENG               512

uint32_t line_num = 1;
static char current_line[MAX_LINE_LENG];
static char string_buf[MAX_LINE_LENG];

static uint32_t opt_src = 1;
static uint32_t opt_tok = 1;
static char *current_line_ptr = current_line;
static char *string_buf_ptr = string_buf;

static void appendToCurrentLine(const char *yytext_ptr);
static void nextLine();

%}

delimiter   [,;:()[\]]
operator    [+\-*/]|mod|:=|<|<=|<>|>=|>|=|and|or|not
keyword     var|def|array|of|boolean|integer|real|string|true|false|while|do|if|then|else|for|to|begin|end|print|read|return
identifier  [a-zA-Z][a-zA-Z0-9]*
nzdec_integer   [1-9][0-9]*
dec_integer     0|{nzdec_integer}
oct_integer     0[0-7]+
nz_fraction     \.([0-9]*[1-9])
fraction        (\.0)|{nz_fraction}
float           {dec_integer}{fraction}
scientific      (({nzdec_integer}{fraction}?)|0{nz_fraction})[eE][+-]?{dec_integer}

%x STRING COMMENT

%%
{delimiter}     LIST_TOKEN(yytext);
{operator}      LIST_TOKEN(yytext);
{keyword}       {
    char* s = malloc(yyleng+2);
    sprintf(s, "KW%s", yytext);
    LIST_TOKEN(s);
    free(s);
}
{identifier}    LIST_LITERAL("id", yytext);
{dec_integer}   LIST_LITERAL("integer", yytext);
{oct_integer}   LIST_LITERAL("oct_integer", yytext);
{float}         LIST_LITERAL("float", yytext);
{scientific}    LIST_LITERAL("scientific", yytext);

\"              {
    LIST_SOURCE;
    string_buf_ptr = string_buf;
    BEGIN(STRING);
}
<STRING>[^"]*   {
    LIST_SOURCE;
    memcpy(string_buf_ptr, yytext, yyleng);
    string_buf_ptr += yyleng;
}
<STRING>\"\"    {
    LIST_SOURCE;
    *string_buf_ptr++ = '"';
}
<STRING>\"      {
    *string_buf_ptr++ = '\0';
    LIST_LITERAL("string", string_buf);
    BEGIN(INITIAL);
}
<STRING><<EOF>> BEGIN(INITIAL);

"//&S+"[^\n]*           { opt_src = 1; REJECT; }
"//&S-"[^\n]*           { opt_src = 0; REJECT; }
"//&T+"[^\n]*           { opt_tok = 1; REJECT; }
"//&T-"[^\n]*           { opt_tok = 0; REJECT; }
"//"[^\n]*              LIST_SOURCE;
"/*" 		            LIST_SOURCE, BEGIN(COMMENT);
<COMMENT>[^*\n]*        LIST_SOURCE;
<COMMENT>"*"+[^*/\n]*   LIST_SOURCE;
<COMMENT>\n             nextLine();
<COMMENT>"*"+"/"        LIST_SOURCE, BEGIN(INITIAL);

[ \t] LIST_SOURCE;
<*>\n nextLine();

    /* Catch the character which is not accepted by rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}

%%

static void appendToCurrentLine(const char *yytext_ptr) {
    while (*yytext_ptr) {
        *current_line_ptr = *yytext_ptr;
        ++current_line_ptr;
        ++yytext_ptr;
    }
    *current_line_ptr = '\0';
}

static void nextLine() {
    if (opt_src) {
        printf("%d: %s\n", line_num, current_line);
    }
    ++line_num;
    current_line[0] = '\0';
    current_line_ptr = current_line;
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yylex();

    fclose(yyin);
    /* Free resources used by the scanner; otherwise, unused data structures
    will persist throughout the successive compiler phases. */
    yylex_destroy();

    return 0;
}
