#include <cutter.h>
#include "mysh/mysh.h"

void
test_mysh_parser_parse_command_without_args(void)
{
        Mysh_Lexer lexer;
        Mysh_Parser parser;
        Mysh_Pipeline *pipeline;
        
        mysh_lexer_init(&lexer, "ls");
        mysh_parser_init(&parser, &lexer);

        cut_assert_equal_int(MYSH_SUCCESS, mysh_parser_parse(&parser, &pipeline));
        cut_assert_equal_string("ls", pipeline->commandv[0]->file);

        mysh_pipeline_delete(pipeline);
}

void
test_mysh_parser_parse_pipeline_without_args(void)
{
        Mysh_Lexer lexer;
        Mysh_Parser parser;
        Mysh_Pipeline *pipeline;
        
        mysh_lexer_init(&lexer, "ls | sort | head");
        mysh_parser_init(&parser, &lexer);

        cut_assert_equal_int(MYSH_SUCCESS, mysh_parser_parse(&parser, &pipeline));
        cut_assert_equal_string("ls", pipeline->commandv[0]->file);
        cut_assert_equal_string("sort", pipeline->commandv[1]->file);
        cut_assert_equal_string("head", pipeline->commandv[2]->file);

        mysh_pipeline_delete(pipeline);
}

void
test_mysh_parser_parse_pipeline_with_argv0(void)
{
        Mysh_Lexer lexer;
        Mysh_Parser parser;
        Mysh_Pipeline *pipeline;
        
        mysh_lexer_init(&lexer, "ls | sort | head");
        mysh_parser_init(&parser, &lexer);

        cut_assert_equal_int(MYSH_SUCCESS, mysh_parser_parse(&parser, &pipeline));
        cut_assert_equal_string("ls", pipeline->commandv[0]->file);
        cut_assert_equal_string("ls", pipeline->commandv[0]->argv[0]);
        cut_assert_null(pipeline->commandv[0]->argv[1]);
        cut_assert_equal_string("sort", pipeline->commandv[1]->file);
        cut_assert_equal_string("sort", pipeline->commandv[1]->argv[0]);
        cut_assert_null(pipeline->commandv[1]->argv[1]);
        cut_assert_equal_string("head", pipeline->commandv[2]->file);
        cut_assert_equal_string("head", pipeline->commandv[2]->argv[0]);
        cut_assert_null(pipeline->commandv[2]->argv[1]);

        mysh_pipeline_delete(pipeline);
}

void
test_mysh_parser_parse_pipeline_with_args(void)
{
        Mysh_Lexer lexer;
        Mysh_Parser parser;
        Mysh_Pipeline *pipeline;
        
        mysh_lexer_init(&lexer, "ls -l | sort | head -n 20");
        mysh_parser_init(&parser, &lexer);

        cut_assert_equal_int(MYSH_SUCCESS, mysh_parser_parse(&parser, &pipeline));
        cut_assert_equal_string("ls", pipeline->commandv[0]->file);
        cut_assert_equal_string("ls", pipeline->commandv[0]->argv[0]);
        cut_assert_equal_string("-l", pipeline->commandv[0]->argv[1]);
        cut_assert_null(pipeline->commandv[0]->argv[2]);
        cut_assert_equal_string("sort", pipeline->commandv[1]->file);
        cut_assert_equal_string("sort", pipeline->commandv[1]->argv[0]);
        cut_assert_null(pipeline->commandv[1]->argv[1]);
        cut_assert_equal_string("head", pipeline->commandv[2]->file);
        cut_assert_equal_string("head", pipeline->commandv[2]->argv[0]);
        cut_assert_equal_string("-n", pipeline->commandv[2]->argv[1]);
        cut_assert_equal_string("20", pipeline->commandv[2]->argv[2]);
        cut_assert_null(pipeline->commandv[2]->argv[3]);

        mysh_pipeline_delete(pipeline);
}

void
test_mysh_parser_parse_redir_to_file(void)
{
        Mysh_Lexer lexer;
        Mysh_Parser parser;
        Mysh_Pipeline *pipeline;
        Mysh_Command *command;
        
        mysh_lexer_init(&lexer, "command 1>file1 2>file2");
        mysh_parser_init(&parser, &lexer);

        cut_assert_equal_int(MYSH_SUCCESS, mysh_parser_parse(&parser, &pipeline));
        command = pipeline->commandv[0];
        cut_assert_equal_string("command", command->file);
        cut_assert_equal_string("command", command->argv[0]);
        cut_assert_null(command->argv[1]);
        cut_assert_equal_int(1, command->redirv[0]->src_fd);
        cut_assert_equal_int(MYSH_REDIR_TO_FILE, command->redirv[0]->dst_type);
        cut_assert_equal_string("file1", command->redirv[0]->dst.file);
        cut_assert_equal_int(2, command->redirv[1]->src_fd);
        cut_assert_equal_int(MYSH_REDIR_TO_FILE, command->redirv[1]->dst_type);
        cut_assert_equal_string("file2", command->redirv[1]->dst.file);

        mysh_pipeline_delete(pipeline);
}

void
test_mysh_parser_parse_redir_to_fd(void)
{
        Mysh_Lexer lexer;
        Mysh_Parser parser;
        Mysh_Pipeline *pipeline;
        Mysh_Command *command;
        
        mysh_lexer_init(&lexer, "command 2>&1");
        mysh_parser_init(&parser, &lexer);

        cut_assert_equal_int(MYSH_SUCCESS, mysh_parser_parse(&parser, &pipeline));
        command = pipeline->commandv[0];
        cut_assert_equal_string("command", command->file);
        cut_assert_equal_string("command", command->argv[0]);
        cut_assert_null(command->argv[1]);
        cut_assert_equal_int(2, command->redirv[0]->src_fd);
        cut_assert_equal_int(MYSH_REDIR_TO_FD, command->redirv[0]->dst_type);
        cut_assert_equal_int(1, command->redirv[0]->dst.fd);

        mysh_pipeline_delete(pipeline);
}

void
test_mysh_parser_parse_out_and_err_to_pipe(void)
{
        Mysh_Lexer lexer;
        Mysh_Parser parser;
        Mysh_Pipeline *pipeline;
        Mysh_Command *command;
        
        mysh_lexer_init(&lexer, "command 2>&1 | less");
        mysh_parser_init(&parser, &lexer);

        cut_assert_equal_int(MYSH_SUCCESS, mysh_parser_parse(&parser, &pipeline));
        command = pipeline->commandv[0];
        cut_assert_equal_string("command", command->file);
        cut_assert_equal_string("command", command->argv[0]);
        cut_assert_null(command->argv[1]);
        cut_assert_equal_int(2, command->redirv[0]->src_fd);
        cut_assert_equal_int(MYSH_REDIR_TO_FD, command->redirv[0]->dst_type);
        cut_assert_equal_int(1, command->redirv[0]->dst.fd);
        command = pipeline->commandv[1];
        cut_assert_equal_string("less", command->file);
        cut_assert_equal_string("less", command->argv[0]);
        cut_assert_null(command->argv[1]);

        mysh_pipeline_delete(pipeline);
}

void
test_mysh_parser_parse_out_and_err_to_file(void)
{
        Mysh_Lexer lexer;
        Mysh_Parser parser;
        Mysh_Pipeline *pipeline;
        Mysh_Command *command;
        
        mysh_lexer_init(&lexer, "command >file 2>&1");
        mysh_parser_init(&parser, &lexer);

        cut_assert_equal_int(MYSH_SUCCESS, mysh_parser_parse(&parser, &pipeline));
        command = pipeline->commandv[0];
        cut_assert_equal_string("command", command->file);
        cut_assert_equal_string("command", command->argv[0]);
        cut_assert_null(command->argv[1]);
        cut_assert_equal_int(1, command->redirv[0]->src_fd);
        cut_assert_equal_int(MYSH_REDIR_TO_FILE, command->redirv[0]->dst_type);
        cut_assert_equal_string("file", command->redirv[0]->dst.file);
        cut_assert_equal_int(2, command->redirv[1]->src_fd);
        cut_assert_equal_int(MYSH_REDIR_TO_FD, command->redirv[1]->dst_type);
        cut_assert_equal_int(1, command->redirv[1]->dst.fd);

        mysh_pipeline_delete(pipeline);
}

void
test_mysh_parser_parse_less_err_only(void)
{
        Mysh_Lexer lexer;
        Mysh_Parser parser;
        Mysh_Pipeline *pipeline;
        Mysh_Command *command;
        
        mysh_lexer_init(&lexer, "command 2>&1 1>/dev/null | less");
        mysh_parser_init(&parser, &lexer);

        cut_assert_equal_int(MYSH_SUCCESS, mysh_parser_parse(&parser, &pipeline));
        command = pipeline->commandv[0];
        cut_assert_equal_string("command", command->file);
        cut_assert_equal_string("command", command->argv[0]);
        cut_assert_null(command->argv[1]);
        cut_assert_equal_int(2, command->redirv[0]->src_fd);
        cut_assert_equal_int(MYSH_REDIR_TO_FD, command->redirv[0]->dst_type);
        cut_assert_equal_int(1, command->redirv[0]->dst.fd);
        cut_assert_equal_int(1, command->redirv[1]->src_fd);
        cut_assert_equal_int(MYSH_REDIR_TO_FILE, command->redirv[1]->dst_type);
        cut_assert_equal_string("/dev/null", command->redirv[1]->dst.file);
        command = pipeline->commandv[1];
        cut_assert_equal_string("less", command->file);
        cut_assert_equal_string("less", command->argv[0]);
        cut_assert_null(command->argv[1]);

        mysh_pipeline_delete(pipeline);
}

void
test_mysh_parser_parse_swap_out_and_err(void)
{
        Mysh_Lexer lexer;
        Mysh_Parser parser;
        Mysh_Pipeline *pipeline;
        Mysh_Command *command;
        
        mysh_lexer_init(&lexer, "command 3>&1 1>&2 2>&3");
        mysh_parser_init(&parser, &lexer);

        cut_assert_equal_int(MYSH_SUCCESS, mysh_parser_parse(&parser, &pipeline));
        command = pipeline->commandv[0];
        cut_assert_equal_string("command", command->file);
        cut_assert_equal_string("command", command->argv[0]);
        cut_assert_null(command->argv[1]);
        cut_assert_equal_int(3, command->redirv[0]->src_fd);
        cut_assert_equal_int(MYSH_REDIR_TO_FD, command->redirv[0]->dst_type);
        cut_assert_equal_int(1, command->redirv[0]->dst.fd);
        cut_assert_equal_int(1, command->redirv[1]->src_fd);
        cut_assert_equal_int(MYSH_REDIR_TO_FD, command->redirv[1]->dst_type);
        cut_assert_equal_int(2, command->redirv[1]->dst.fd);
        cut_assert_equal_int(2, command->redirv[2]->src_fd);
        cut_assert_equal_int(MYSH_REDIR_TO_FD, command->redirv[2]->dst_type);
        cut_assert_equal_int(3, command->redirv[2]->dst.fd);

        mysh_pipeline_delete(pipeline);
}

void
test_mysh_parser_parse_append(void)
{
        Mysh_Lexer lexer;
        Mysh_Parser parser;
        Mysh_Pipeline *pipeline;
        Mysh_Command *command;
        Mysh_Redir *redir;
        
        mysh_lexer_init(&lexer, "cat data1 | sort >> result");
        mysh_parser_init(&parser, &lexer);

        cut_assert_equal_int(MYSH_SUCCESS, mysh_parser_parse(&parser, &pipeline));
        command = pipeline->commandv[0];
        cut_assert_equal_string("cat", command->file);
        cut_assert_equal_string("cat", command->argv[0]);
        cut_assert_equal_string("data1", command->argv[1]);
        cut_assert_null(command->argv[2]);
        command = pipeline->commandv[1];
        cut_assert_equal_string("sort", command->file);
        cut_assert_equal_string("sort", command->argv[0]);
        cut_assert_null(command->argv[1]);
        redir = command->redirv[0];
        cut_assert_equal_int(1, redir->src_fd);
        cut_assert_equal_int(MYSH_REDIR_TO_FILE_APPEND, redir->dst_type);
        cut_assert_equal_string("result", redir->dst.file);
        
        mysh_pipeline_delete(pipeline);
}
