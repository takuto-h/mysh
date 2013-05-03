#ifndef MYSH_PARSER_H_INCLUDED
#define MYSH_PARSER_H_INCLUDED

typedef struct {
        Mysh_Lexer *lexer;
        Mysh_Token token;
} Mysh_Parser;

void mysh_parser_init(Mysh_Parser *parser, Mysh_Lexer *lexer);
Mysh_Result mysh_parser_parse(Mysh_Parser *parser, Mysh_Pipeline **pipelinep);

#endif /* MYSH_PARSER_H_INCLUDED */
