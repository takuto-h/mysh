#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "mysh/mysh.h"

static void run_pipeline(Mysh_Pipeline *pipeline);
static pid_t run_command(Mysh_Command *command,
                         int *pipefdv, int pipefdc, int infd_idx, int outfd_idx);
static void builtin_exit(Mysh_Command *command);
static void builtin_cd(Mysh_Command *command);

/* child */
static void setup_pipe(int *pipefdv, int pipefdc, int infd_idx, int outfd_idx);
static void setup_redir(Mysh_Redir **redirv, int redirc);
static void setup_redir_from_file(Mysh_Redir *redir);
static void setup_redir_to_file(Mysh_Redir *redir);
static void setup_redir_to_file_append(Mysh_Redir *redir);
static void setup_redir_to_fd(Mysh_Redir *redir);

void
mysh_runner_run(Mysh_Pipeline *pipeline)
{
        run_pipeline(pipeline);
}

static void
run_pipeline(Mysh_Pipeline *pipeline)
{
        int i;
        int *pipefdv;
        int pipefdc;
        int childc;
        Mysh_Command *command;
        int infd_idx;
        int outfd_idx;
        pid_t child_pid;

        pipefdc = 2 * (pipeline->commandc + 1);
        pipefdv = (int *)malloc(pipefdc * sizeof(int));
        if (pipefdv == NULL) {
                mysh_perror("malloc");
                exit(EXIT_FAILURE);
        }
        pipefdv[0] = -1;
        pipefdv[1] = -1;
        pipefdv[pipefdc - 2] = -1;
        pipefdv[pipefdc - 1] = -1;
        for (i = 1; i < pipeline->commandc; i++) {
                if (pipe(pipefdv + 2 * i) < 0) {
                        mysh_perror("pipe");
                        exit(EXIT_FAILURE);
                }
        }
        childc = 0;
        for (i = 0; i < pipeline->commandc; i++) {
                command = pipeline->commandv[i];
                infd_idx = 2 * i;
                outfd_idx = 2 * (i + 1) + 1;
                child_pid = run_command(command,
                                        pipefdv, pipefdc, infd_idx, outfd_idx);
                if (child_pid != 0) {
                        childc++;
                }
        }
        for (i = 1; i < pipeline->commandc; i++) {
                if (close(pipefdv[2 * i]) < 0) {
                        mysh_perror("close: %d", pipefdv[2 * i]);
                        exit(EXIT_FAILURE);
                }
                if (close(pipefdv[2 * i + 1]) < 0) {
                        mysh_perror("close: %d", pipefdv[2 * i + 1]);
                        exit(EXIT_FAILURE);
                }
        }
        for (i = 0; i < childc; i++) {
                if (waitpid(-1, NULL, 0) < 0) {
                        mysh_perror("waitpid");
                        exit(EXIT_FAILURE);
                }
        }
        free(pipefdv);
}

static pid_t
run_command(Mysh_Command *command,
            int *pipefdv, int pipefdc, int infd_idx, int outfd_idx)
{
        pid_t child_pid;
        
        if (strcmp("exit", command->file) == 0) {
                builtin_exit(command);
                return 0;
        }
        if (strcmp("cd", command->file) == 0) {
                builtin_cd(command);
                return 0;
        }
        child_pid = fork();
        if (child_pid < 0) {
                mysh_perror("fork");
                exit(EXIT_FAILURE);
        }
        if (child_pid == 0) {
                /* child */
                setup_pipe(pipefdv, pipefdc, infd_idx, outfd_idx);
                setup_redir(command->redirv, command->redirc);
                execvp(command->file, command->argv);
                mysh_perror("execvp: %s", command->file);
                _exit(EXIT_FAILURE);
        }
        return child_pid;
}

static void
builtin_exit(Mysh_Command *command)
{
        const char *arg;
        int status;
        char *endptr;
        
        switch (command->argc) {
        case 1:
                status = EXIT_SUCCESS;
                break;
        case 2:
                arg = command->argv[1];
                status = strtol(arg, &endptr, 10);
                if (*endptr != '\0') {
                        mysh_display("exit: %s: numeric argument required\n", arg);
                        return;
                }
                break;
        default:
                mysh_display("exit: too many arguments\n");
                return;
        }
        exit(status);
}

static void
builtin_cd(Mysh_Command *command)
{
        const char *arg;
        
        switch (command->argc) {
        case 1:
                mysh_display("cd: too few arguments\n");
                return;
        case 2:
                arg = command->argv[1];
                break;
        default:
                mysh_display("cd: too many arguments\n");
                return;
        }
        if (chdir(arg) < 0) {
                mysh_perror("cd: %s", arg);
                return;
        }
}

static void
setup_pipe(int *pipefdv, int pipefdc, int infd_idx, int outfd_idx)
{
        /* child */
        int i;
        
        for (i = 0; i < pipefdc; i++) {
                if (pipefdv[i] == -1) {
                        continue;
                }
                if (i == infd_idx) {
                        if (dup2(pipefdv[i], 0) < 0) {
                                mysh_perror("dup2: %d", pipefdv[i]);
                                _exit(EXIT_FAILURE);
                        }
                }
                if (i == outfd_idx) {
                        if (dup2(pipefdv[i], 1) < 0) {
                                mysh_perror("dup2: %d", pipefdv[i]);
                                _exit(EXIT_FAILURE);
                        }
                }
                if (close(pipefdv[i]) < 0) {
                        mysh_perror("close: %d", pipefdv[i]);
                        _exit(EXIT_FAILURE);
                }
        }
}

static void
setup_redir(Mysh_Redir **redirv, int redirc)
{
        /* child */
        int i;
        
        for (i = 0; i < redirc; i++) {
                switch (redirv[i]->dst_type) {
                case MYSH_REDIR_FROM_FILE:
                        setup_redir_from_file(redirv[i]);
                        break;
                case MYSH_REDIR_TO_FILE:
                        setup_redir_to_file(redirv[i]);
                        break;
                case MYSH_REDIR_TO_FILE_APPEND:
                        setup_redir_to_file_append(redirv[i]);
                        break;
                case MYSH_REDIR_TO_FD:
                        setup_redir_to_fd(redirv[i]);
                        break;
                }
        }
}

static void
setup_redir_from_file(Mysh_Redir *redir)
{
        /* child */
        int dst_fd;
        
        dst_fd = open(redir->dst.file, O_RDONLY);
        if (dst_fd < 0) {
                mysh_perror("open: %s", redir->dst.file);
                _exit(EXIT_FAILURE);
        }
        if (dup2(dst_fd, redir->src_fd) < 0) {
                mysh_perror("dup2");
                _exit(EXIT_FAILURE);
        }
        if (close(dst_fd) < 0) {
                mysh_perror("close: %d", dst_fd);
                _exit(EXIT_FAILURE);
        }
}

static void
setup_redir_to_file(Mysh_Redir *redir)
{
        /* child */
        int dst_fd;

        dst_fd = open(redir->dst.file, O_WRONLY | O_CREAT | O_TRUNC, 00666);
        if (dst_fd < 0) {
                mysh_perror("open: %s", redir->dst.file);
                _exit(EXIT_FAILURE);
        }
        if (dup2(dst_fd, redir->src_fd) < 0) {
                mysh_perror("dup2");
                _exit(EXIT_FAILURE);
        }
        if (close(dst_fd) < 0) {
                mysh_perror("close: %d", dst_fd);
                _exit(EXIT_FAILURE);
        }
}

static void
setup_redir_to_file_append(Mysh_Redir *redir)
{
        /* child */
        int dst_fd;

        dst_fd = open(redir->dst.file, O_WRONLY | O_CREAT | O_APPEND, 00666);
        if (dst_fd < 0) {
                mysh_perror("open: %s", redir->dst.file);
                _exit(EXIT_FAILURE);
        }
        if (dup2(dst_fd, redir->src_fd) < 0) {
                mysh_perror("dup2");
                _exit(EXIT_FAILURE);
        }
        if (close(dst_fd) < 0) {
                mysh_perror("close: %d", dst_fd);
                _exit(EXIT_FAILURE);
        }               
}

static void
setup_redir_to_fd(Mysh_Redir *redir)
{
        /* child */
        if (dup2(redir->dst.fd, redir->src_fd) < 0) {
                mysh_perror("dup2");
                _exit(EXIT_FAILURE);
        }
}
