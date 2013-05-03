#include <stdlib.h>
#include "mysh/mysh.h"

static Mysh_Token peek_token(Mysh_Parser *parser);
static void junk_token(Mysh_Parser *parser);
static Mysh_Result parse_pipeline(Mysh_Parser *parser, Mysh_Pipeline **pipelinep);
static Mysh_Result parse_command(Mysh_Parser *parser, Mysh_Command **commandp);
static Mysh_Result parse_arg_or_redir(Mysh_Parser *parser, Mysh_Command *command);
static Mysh_Result parse_redir_from_file(Mysh_Parser *parser, Mysh_Command *command);
static Mysh_Result parse_redir_to_file(Mysh_Parser *parser, Mysh_Command *command);
static Mysh_Result parse_redir_to_file_a(Mysh_Parser *parser, Mysh_Command *command);
static Mysh_Result parse_redir_to_fd(Mysh_Parser *parser, Mysh_Command *command);

void
mysh_parser_init(Mysh_Parser *parser, Mysh_Lexer *lexer)
{
        parser->lexer = lexer;
        parser->token = MYSH_TOKEN_NONE;
}

Mysh_Result
mysh_parser_parse(Mysh_Parser *parser, Mysh_Pipeline **pipelinep)
{
        if (parse_pipeline(parser, pipelinep) == MYSH_FAILURE) {
                return MYSH_FAILURE;
        }
        switch (peek_token(parser)) {
        case MYSH_TOKEN_EOF:
                return MYSH_SUCCESS;
        default:
                return MYSH_FAILURE;
        }
}

static Mysh_Result
parse_pipeline(Mysh_Parser *parser, Mysh_Pipeline **pipelinep)
{
        Mysh_Command *command;
        
        if (parse_command(parser, &command) == MYSH_FAILURE) {
                return MYSH_FAILURE;
        }
        *pipelinep = mysh_pipeline_new(command);
        while (peek_token(parser) == MYSH_TOKEN_PIPE) {
                junk_token(parser);
                if (parse_command(parser, &command) == MYSH_FAILURE) {
                        mysh_pipeline_delete(*pipelinep);
                        return MYSH_FAILURE;
                }
                mysh_pipeline_add(*pipelinep, command);
        }
        return MYSH_SUCCESS;
}

static Mysh_Result
parse_command(Mysh_Parser *parser, Mysh_Command **commandp)
{
        switch (peek_token(parser)) {
        case MYSH_TOKEN_WORD:
                *commandp = mysh_command_new(parser->lexer->value.v_str);
                junk_token(parser);
                while (peek_token(parser) == MYSH_TOKEN_WORD ||
                       peek_token(parser) == MYSH_TOKEN_REDIR_FROM_FILE ||
                       peek_token(parser) == MYSH_TOKEN_REDIR_TO_FILE ||
                       peek_token(parser) == MYSH_TOKEN_REDIR_TO_FILE_APPEND ||
                       peek_token(parser) == MYSH_TOKEN_REDIR_TO_FD) {
                        parse_arg_or_redir(parser, *commandp);
                }
                return MYSH_SUCCESS;
        default:
                return MYSH_FAILURE;
        }
}

static Mysh_Result
parse_arg_or_redir(Mysh_Parser *parser, Mysh_Command *command)
{
        switch (peek_token(parser)) {
        case MYSH_TOKEN_WORD:
                mysh_command_add_arg(command, parser->lexer->value.v_str);
                junk_token(parser);
                break;
        case MYSH_TOKEN_REDIR_FROM_FILE:
                if (parse_redir_from_file(parser, command) == MYSH_FAILURE) {
                        mysh_command_delete(command);
                        return MYSH_FAILURE;
                }
                break;
        case MYSH_TOKEN_REDIR_TO_FILE:
                if (parse_redir_to_file(parser, command) == MYSH_FAILURE) {
                        mysh_command_delete(command);
                        return MYSH_FAILURE;
                }
                break;
        case MYSH_TOKEN_REDIR_TO_FILE_APPEND:
                if (parse_redir_to_file_a(parser, command) == MYSH_FAILURE) {
                        mysh_command_delete(command);
                        return MYSH_FAILURE;
                }
                break;
        case MYSH_TOKEN_REDIR_TO_FD:
                if (parse_redir_to_fd(parser, command) == MYSH_FAILURE) {
                        mysh_command_delete(command);
                        return MYSH_FAILURE;
                }
                break;
        default:
                return MYSH_FAILURE;
        }
        return MYSH_SUCCESS;
}


static Mysh_Result
parse_redir_from_file(Mysh_Parser *parser, Mysh_Command *command)
{
        int src_fd;
        const char *dst_file;
        Mysh_Redir *redir;
        
        src_fd = strtol(parser->lexer->value.v_str, NULL, 10);
        junk_token(parser);
        switch (peek_token(parser)) {
        case MYSH_TOKEN_WORD:
                dst_file = parser->lexer->value.v_str;
                redir = mysh_redir_from_file_new(src_fd, dst_file);
                mysh_command_add_redir(command, redir);
                junk_token(parser);
                return MYSH_SUCCESS;
        default:
                return MYSH_FAILURE;
        }
}

static Mysh_Result
parse_redir_to_file(Mysh_Parser *parser, Mysh_Command *command)
{
        int src_fd;
        const char *dst_file;
        Mysh_Redir *redir;
        
        src_fd = strtol(parser->lexer->value.v_str, NULL, 10);
        junk_token(parser);
        switch (peek_token(parser)) {
        case MYSH_TOKEN_WORD:
                dst_file = parser->lexer->value.v_str;
                redir = mysh_redir_to_file_new(src_fd, dst_file);
                mysh_command_add_redir(command, redir);
                junk_token(parser);
                return MYSH_SUCCESS;
        default:
                return MYSH_FAILURE;
        }
}

static Mysh_Result
parse_redir_to_file_a(Mysh_Parser *parser, Mysh_Command *command)
{
        int src_fd;
        const char *dst_file;
        Mysh_Redir *redir;
        
        src_fd = strtol(parser->lexer->value.v_str, NULL, 10);
        junk_token(parser);
        switch (peek_token(parser)) {
        case MYSH_TOKEN_WORD:
                dst_file = parser->lexer->value.v_str;
                redir = mysh_redir_to_file_append_new(src_fd, dst_file);
                mysh_command_add_redir(command, redir);
                junk_token(parser);
                return MYSH_SUCCESS;
        default:
                return MYSH_FAILURE;
        }
}

static Mysh_Result
parse_redir_to_fd(Mysh_Parser *parser, Mysh_Command *command)
{
        int src_fd;
        int dst_fd;
        char *endptr;
        Mysh_Redir *redir;
        
        src_fd = strtol(parser->lexer->value.v_str, NULL, 10);
        junk_token(parser);
        switch (peek_token(parser)) {
        case MYSH_TOKEN_WORD:
                dst_fd = strtol(parser->lexer->value.v_str, &endptr, 10);
                if (*endptr != '\0') {
                        return MYSH_FAILURE;
                }
                redir = mysh_redir_to_fd_new(src_fd, dst_fd);
                mysh_command_add_redir(command, redir);
                junk_token(parser);
                return MYSH_SUCCESS;
        default:
                return MYSH_FAILURE;
        }
}

static Mysh_Token
peek_token(Mysh_Parser *parser)
{
        if (parser->token == MYSH_TOKEN_NONE) {
                junk_token(parser);
        }
        return parser->token;
}

static void
junk_token(Mysh_Parser *parser)
{
        mysh_lexer_lex(parser->lexer);
        parser->token = parser->lexer->token;
}
