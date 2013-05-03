#include <stdio.h>
#include <ctype.h>
#include "mysh/mysh.h"

static int isseparator(char c);
static char peek_char(Mysh_Lexer *lexer);
static void junk_char(Mysh_Lexer *lexer);
static void lex_less(Mysh_Lexer *lexer);
static void lex_greater(Mysh_Lexer *lexer);
static void lex_quote(Mysh_Lexer *lexer, char quote);
static void lex_digits(Mysh_Lexer *lexer, char *cp);
static void lex_word(Mysh_Lexer *lexer, char *cp);
static void skip_spaces(Mysh_Lexer *lexer);

void
mysh_lexer_init(Mysh_Lexer *lexer, const char *buff)
{
        lexer->buff = buff;
}

static int
isseparator(char c)
{
        return c == '<' || c == '>' || c == '|' || c == '"' || c == '\'';
}

void
mysh_lexer_lex(Mysh_Lexer *lexer)
{
        skip_spaces(lexer);
        switch (peek_char(lexer)) {
        case '\0':
                lexer->token = MYSH_TOKEN_EOF;
                break;
        case '<':
                snprintf(lexer->value.v_str, MYSH_WORD_BUFF_SIZE, "0");
                lex_less(lexer);
                break;
        case '>':
                snprintf(lexer->value.v_str, MYSH_WORD_BUFF_SIZE, "1");
                lex_greater(lexer);
                break;
        case '|':
                lexer->token = MYSH_TOKEN_PIPE;
                junk_char(lexer);
                break;
        case '"':
        case '\'':
                lex_quote(lexer, peek_char(lexer));
                break;
        default:
                snprintf(lexer->value.v_str, MYSH_WORD_BUFF_SIZE, "%s", lexer->buff);
                if (isdigit(peek_char(lexer))) {
                        lex_digits(lexer, lexer->value.v_str);
                        break;
                }
                lex_word(lexer, lexer->value.v_str);
                break;
        }
}

static void
lex_less(Mysh_Lexer *lexer)
{
        lexer->token = MYSH_TOKEN_REDIR_FROM_FILE;
        junk_char(lexer);
        switch (peek_char(lexer)) {
        case '&':
                lexer->token = MYSH_TOKEN_REDIR_TO_FD;
                junk_char(lexer);
                break;
        }
}

static void
lex_greater(Mysh_Lexer *lexer)
{
        lexer->token = MYSH_TOKEN_REDIR_TO_FILE;
        junk_char(lexer);
        switch (peek_char(lexer)) {
        case '&':
                lexer->token = MYSH_TOKEN_REDIR_TO_FD;
                junk_char(lexer);
                break;
        case '>':
                lexer->token = MYSH_TOKEN_REDIR_TO_FILE_APPEND;
                junk_char(lexer);
                break;
        }
}

static void
lex_quote(Mysh_Lexer *lexer, char quote)
{
        char *cp;
        
        lexer->token = MYSH_TOKEN_WORD;
        junk_char(lexer);
        snprintf(lexer->value.v_str, MYSH_WORD_BUFF_SIZE, "%s", lexer->buff);
        for (cp = lexer->value.v_str; *cp != '\0'; cp++) {
                if (*cp == quote) {
                        *cp = '\0';
                        junk_char(lexer);
                        break;
                }
                junk_char(lexer);
        }
}

static void
lex_digits(Mysh_Lexer *lexer, char *cp)
{        
        for (; *cp != '\0'; cp++) {
                if (*cp == '<') {
                        *cp = '\0';
                        lex_less(lexer);
                        return;
                }
                if (*cp == '>') {
                        *cp = '\0';
                        lex_greater(lexer);
                        return;
                }
                if (!isdigit(*cp)) {
                        break;
                }
                junk_char(lexer);
        }
        lex_word(lexer, cp);
}

static void
lex_word(Mysh_Lexer *lexer, char *cp)
{
        lexer->token = MYSH_TOKEN_WORD;
        for (; *cp != '\0'; cp++) {
                if (isspace(*cp) || isseparator(*cp)) {
                        *cp = '\0';
                        break;
                }
                junk_char(lexer);
        }
}

static void
skip_spaces(Mysh_Lexer *lexer)
{
        for (; peek_char(lexer) != '\0'; junk_char(lexer)) {
                if (!isspace(peek_char(lexer))) {
                        break;
                }
        }
}

static char
peek_char(Mysh_Lexer *lexer)
{
        return *lexer->buff;
}

static void
junk_char(Mysh_Lexer *lexer)
{
        lexer->buff++;
}
