#ifndef MYSH_LEXER_H_INCLUDED
#define MYSH_LEXER_H_INCLUDED

typedef enum {
        MYSH_TOKEN_NONE,
        MYSH_TOKEN_EOF,
        MYSH_TOKEN_WORD,
        MYSH_TOKEN_REDIR_FROM_FILE,
        MYSH_TOKEN_REDIR_TO_FILE,
        MYSH_TOKEN_REDIR_TO_FILE_APPEND,
        MYSH_TOKEN_REDIR_TO_FD,
        MYSH_TOKEN_PIPE
} Mysh_Token;

typedef struct {
        const char *buff;
        Mysh_Token token;
        union {
                char v_str[MYSH_WORD_BUFF_SIZE];
        } value;
} Mysh_Lexer;

void mysh_lexer_init(Mysh_Lexer *lexer, const char *buff);
void mysh_lexer_lex(Mysh_Lexer *lexer);

#endif /* MYSH_LEXER_H_INCLUDED */
