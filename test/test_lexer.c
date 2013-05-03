#include <cutter.h>
#include "mysh/mysh.h"

void
test_mysh_lexer_lex_word(void)
{
        Mysh_Lexer lexer;
        
        mysh_lexer_init(&lexer, "ls");

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("ls", lexer.value.v_str);
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_EOF, lexer.token);
}

void
test_mysh_lexer_skip_space(void)
{
        Mysh_Lexer lexer;
        
        mysh_lexer_init(&lexer, "ls -l");
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("ls", lexer.value.v_str);
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("-l", lexer.value.v_str);
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_EOF, lexer.token);
}

void
test_mysh_lexer_lex_redir(void)
{
        Mysh_Lexer lexer;
        
        mysh_lexer_init(&lexer, "wc < data > result");
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("wc", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_FROM_FILE, lexer.token);
        cut_assert_equal_string("0", lexer.value.v_str);
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("data", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FILE, lexer.token);
        cut_assert_equal_string("1", lexer.value.v_str);
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("result", lexer.value.v_str);
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_EOF, lexer.token);
}

void
test_mysh_lexer_lex_pipe(void)
{
        Mysh_Lexer lexer;
        
        mysh_lexer_init(&lexer, "cat data1 data2 | sort +1 -2 | lpr");
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("cat", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("data1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("data2", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_PIPE, lexer.token);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("sort", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("+1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("-2", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_PIPE, lexer.token);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("lpr", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_EOF, lexer.token);
}

void
test_mysh_lexer_lex_pipe_redir(void)
{
        Mysh_Lexer lexer;
        
        mysh_lexer_init(&lexer, "cat data1 data2 | sort +1 -2 > result");
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("cat", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("data1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("data2", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_PIPE, lexer.token);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("sort", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("+1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("-2", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FILE, lexer.token);
        cut_assert_equal_string("1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("result", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_EOF, lexer.token);
}

void
test_mysh_lexer_lex_no_space_pipe_redir(void)
{
        Mysh_Lexer lexer;
        
        mysh_lexer_init(&lexer, "cat data1 data2|sort +1 -2>result");
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("cat", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("data1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("data2", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_PIPE, lexer.token);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("sort", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("+1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("-2", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FILE, lexer.token);
        cut_assert_equal_string("1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("result", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_EOF, lexer.token);
}

void
test_mysh_lexer_lex_fd_redir(void)
{
        Mysh_Lexer lexer;
        
        mysh_lexer_init(&lexer, "command 1>file1 2>file2");

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("command", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FILE, lexer.token);
        cut_assert_equal_string("1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("file1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FILE, lexer.token);
        cut_assert_equal_string("2", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("file2", lexer.value.v_str);
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_EOF, lexer.token);
}

void
test_mysh_lexer_lex_redir_fd(void)
{
        Mysh_Lexer lexer;
        
        mysh_lexer_init(&lexer, "command 2>&1 | less");

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("command", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FD, lexer.token);
        cut_assert_equal_string("2", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_PIPE, lexer.token);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("less", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_EOF, lexer.token);
}

void
test_mysh_lexer_lex_out_and_err_to_file(void)
{
        Mysh_Lexer lexer;
        
        mysh_lexer_init(&lexer, "command >file 2>&1");

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("command", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FILE, lexer.token);
        cut_assert_equal_string("1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("file", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FD, lexer.token);
        cut_assert_equal_string("2", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_EOF, lexer.token);
}

void
test_mysh_lexer_lex_less_err_only(void)
{
        Mysh_Lexer lexer;
        
        mysh_lexer_init(&lexer, "command 2>&1 1>/dev/null | less");

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("command", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FD, lexer.token);
        cut_assert_equal_string("2", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FILE, lexer.token);
        cut_assert_equal_string("1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("/dev/null", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_PIPE, lexer.token);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("less", lexer.value.v_str);
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_EOF, lexer.token);
}

void
test_mysh_lexer_lex_swap_out_and_err(void)
{
        Mysh_Lexer lexer;
        
        mysh_lexer_init(&lexer, "command 3>&1 1>&2 2>&3");

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("command", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FD, lexer.token);
        cut_assert_equal_string("3", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FD, lexer.token);
        cut_assert_equal_string("1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("2", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FD, lexer.token);
        cut_assert_equal_string("2", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("3", lexer.value.v_str);
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_EOF, lexer.token);
}

void
test_mysh_lexer_lex_append(void)
{
        Mysh_Lexer lexer;
        
        mysh_lexer_init(&lexer, "cat data1 | sort >> result");

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("cat", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("data1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_PIPE, lexer.token);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("sort", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_REDIR_TO_FILE_APPEND, lexer.token);
        cut_assert_equal_string("1", lexer.value.v_str);

        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_WORD, lexer.token);
        cut_assert_equal_string("result", lexer.value.v_str);
    
        mysh_lexer_lex(&lexer);
        cut_assert_equal_int(MYSH_TOKEN_EOF, lexer.token);
}
