#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "mysh/mysh.h"

static void setup_sigaction(void);
static void nop(int signum);
static void print_pipeline(Mysh_Pipeline *pipeline);

int
main(void)
{
        char input[MYSH_LINE_BUFF_SIZE];
        Mysh_Lexer lexer;
        Mysh_Parser parser;
        Mysh_Pipeline *pipeline;
        Mysh_Result result;
        
        setup_sigaction();
        for (;;) {
                fprintf(stderr, "mysh> ");
                fgets(input, MYSH_LINE_BUFF_SIZE, stdin);
                mysh_lexer_init(&lexer, input);
                mysh_parser_init(&parser, &lexer);
                result = mysh_parser_parse(&parser, &pipeline);
                if (result == MYSH_FAILURE) {
                        mysh_display("syntax error\n");
                        continue;
                }
                //print_pipeline(pipeline);
                mysh_runner_run(pipeline);
                mysh_pipeline_delete(pipeline);
        }
        return EXIT_SUCCESS;
}

static void
setup_sigaction(void)
{
        struct sigaction act;
        
        act.sa_handler = nop;
        if (sigemptyset(&act.sa_mask) < 0) {
                mysh_perror("sigemptyset");
                exit(EXIT_FAILURE);
        }
        act.sa_flags = SA_RESTART;
        if (sigaction(SIGINT, &act, NULL) < 0) {
                mysh_perror("sigaction");
                exit(EXIT_FAILURE);
        }
        if (sigaction(SIGQUIT, &act, NULL) < 0) {
                mysh_perror("sigaction");
                exit(EXIT_FAILURE);
        }
}

static void
nop(int signum)
{
        (void)signum;
}

static void
print_pipeline(Mysh_Pipeline *pipeline)
{
        int i, j;
        Mysh_Command *command;
        Mysh_Redir *redir;
        
        for (i = 0; i < pipeline->commandc; i++) {
                command = pipeline->commandv[i];
                printf("#%d\n", i);
                printf("  file:\n");
                printf("    %s\n", command->file);
                printf("  argv:\n");
                for (j = 0; j < command->argc; j++) {
                        printf("    %s\n", command->argv[j]);
                }
                printf("  argc:\n");
                printf("    %d\n", command->argc);
                printf("  redirv:\n");
                for (j = 0; j < command->redirc; j++) {
                        redir = command->redirv[j];
                        printf("    src_fd: %d\n", redir->src_fd);
                        switch (redir->dst_type) {
                        case MYSH_REDIR_FROM_FILE:
                                printf("    dst_type: FROM_FILE\n");
                                printf("    dst.file: %s\n", redir->dst.file);
                                break;
                        case MYSH_REDIR_TO_FILE:
                                printf("    dst_type: TO_FILE\n");
                                printf("    dst.file: %s\n", redir->dst.file);
                                break;
                        case MYSH_REDIR_TO_FILE_APPEND:
                                printf("    dst_type: TO_FILE_APPEND\n");
                                printf("    dst.file: %s\n", redir->dst.file);
                                break;
                        case MYSH_REDIR_TO_FD:
                                printf("    dst_type: TO_FD\n");
                                printf("    dst.fd: %d\n", redir->dst.fd);
                                break;
                        }
                }
                printf("  redirc:\n");
                printf("    %d\n", command->redirc);
        }
}
