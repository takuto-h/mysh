#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mysh/mysh.h"

Mysh_Redir *
mysh_redir_from_file_new(int src_fd, const char *dst_file)
{
        Mysh_Redir *redir;
        
        redir = (Mysh_Redir *)malloc(sizeof(Mysh_Redir));
        if (redir == NULL) {
                mysh_perror("malloc");
                exit(EXIT_FAILURE);
        }
        redir->src_fd = src_fd;
        redir->dst_type = MYSH_REDIR_FROM_FILE;
        redir->dst.file = strdup(dst_file);
        if (redir->dst.file == NULL) {
                mysh_perror("strdup");
                exit(EXIT_FAILURE);
        }
        return redir;
}

Mysh_Redir *
mysh_redir_to_file_new(int src_fd, const char *dst_file)
{
        Mysh_Redir *redir;
        
        redir = (Mysh_Redir *)malloc(sizeof(Mysh_Redir));
        if (redir == NULL) {
                mysh_perror("malloc");
                exit(EXIT_FAILURE);
        }
        redir->src_fd = src_fd;
        redir->dst_type = MYSH_REDIR_TO_FILE;
        redir->dst.file = strdup(dst_file);
        if (redir->dst.file == NULL) {
                mysh_perror("strdup");
                exit(EXIT_FAILURE);
        }
        return redir;
}

Mysh_Redir *
mysh_redir_to_file_append_new(int src_fd, const char *dst_file)
{
        Mysh_Redir *redir;
        
        redir = (Mysh_Redir *)malloc(sizeof(Mysh_Redir));
        if (redir == NULL) {
                mysh_perror("malloc");
                exit(EXIT_FAILURE);
        }
        redir->src_fd = src_fd;
        redir->dst_type = MYSH_REDIR_TO_FILE_APPEND;
        redir->dst.file = strdup(dst_file);
        if (redir->dst.file == NULL) {
                mysh_perror("strdup");
                exit(EXIT_FAILURE);
        }
        return redir;
}

Mysh_Redir *
mysh_redir_to_fd_new(int src_fd, int dst_fd)
{
        Mysh_Redir *redir;
        
        redir = (Mysh_Redir *)malloc(sizeof(Mysh_Redir));
        if (redir == NULL) {
                mysh_perror("malloc");
                exit(EXIT_FAILURE);
        }
        redir->src_fd = src_fd;
        redir->dst_type = MYSH_REDIR_TO_FD;
        redir->dst.fd = dst_fd;
        return redir;
}

void
mysh_redir_delete(Mysh_Redir *redir)
{
        if (redir->dst_type == MYSH_REDIR_FROM_FILE ||
            redir->dst_type == MYSH_REDIR_TO_FILE) {
                free(redir->dst.file);
        }
        free(redir);
}

Mysh_Command *
mysh_command_new(const char *file)
{
        Mysh_Command *command;
        
        command = (Mysh_Command *)malloc(sizeof(Mysh_Command));
        if (command == NULL) {
                mysh_perror("malloc");
                exit(EXIT_FAILURE);
        }
        command->file = strdup(file);
        if (command->file == NULL) {
                mysh_perror("strdup");
                exit(EXIT_FAILURE);
        }
        command->argv = (char **)malloc(1 * sizeof(char *));
        if (command->argv == NULL) {
                mysh_perror("malloc");
                exit(EXIT_FAILURE);
        }
        command->argv[0] = NULL;
        command->argc = 0;
        mysh_command_add_arg(command, file);
        command->redirv = (Mysh_Redir **)malloc(1 * sizeof(Mysh_Redir *));
        if (command->redirv == NULL) {
                mysh_perror("malloc");
                exit(EXIT_FAILURE);
        }
        command->redirv[0] = NULL;
        command->redirc = 0;
        return command;
}

void
mysh_command_delete(Mysh_Command *command)
{
        int i;
        
        free(command->file);
        for (i = 0; i < command->argc; i++) {
                free(command->argv[i]);
        }
        free(command->argv);
        for (i = 0; i < command->redirc; i++) {
                mysh_redir_delete(command->redirv[i]);
        }
        free(command->redirv);
        free(command);
}

void
mysh_command_add_arg(Mysh_Command *command, const char *arg)
{
        size_t new_size;
        
        command->argc++;
        new_size = (command->argc + 1) * sizeof(char *);
        command->argv = (char **)realloc(command->argv, new_size);
        if (command->argv == NULL) {
                mysh_perror("realloc");
                exit(EXIT_FAILURE);
        }
        command->argv[command->argc - 1] = strdup(arg);
        if (command->argv[command->argc - 1] == NULL) {
                mysh_perror("strdup");
                exit(EXIT_FAILURE);
        }
        command->argv[command->argc] = NULL;
}

void
mysh_command_add_redir(Mysh_Command *command, Mysh_Redir *redir)
{
        size_t new_size;
        
        command->redirc++;
        new_size = (command->redirc + 1) * sizeof(Mysh_Redir *);
        command->redirv = (Mysh_Redir **)realloc(command->redirv, new_size);
        if (command->redirv == NULL) {
                mysh_perror("realloc");
                exit(EXIT_FAILURE);
        }
        command->redirv[command->redirc - 1] = redir;
        command->redirv[command->redirc] = NULL;
}

Mysh_Pipeline *
mysh_pipeline_new(Mysh_Command *command)
{
        Mysh_Pipeline *pipeline;
        
        pipeline = (Mysh_Pipeline *)malloc(sizeof(Mysh_Pipeline));
        if (pipeline == NULL) {
                mysh_perror("malloc");
                exit(EXIT_FAILURE);
        }
        pipeline->commandv = (Mysh_Command **)malloc(2 * sizeof(Mysh_Command *));
        if (pipeline->commandv == NULL) {
                mysh_perror("malloc");
                exit(EXIT_FAILURE);
        }
        pipeline->commandv[0] = command;
        pipeline->commandv[1] = NULL;
        pipeline->commandc = 1;
        return pipeline;
}

void
mysh_pipeline_delete(Mysh_Pipeline *pipeline)
{
        int i;
        
        for (i = 0; i < pipeline->commandc; i++) {
                mysh_command_delete(pipeline->commandv[i]);
        }
        free(pipeline->commandv);
        free(pipeline);
}

void
mysh_pipeline_add(Mysh_Pipeline *pipeline, Mysh_Command *command)
{
        size_t new_size;
        
        pipeline->commandc++;
        new_size = (pipeline->commandc + 1) * sizeof(Mysh_Command *);
        pipeline->commandv = (Mysh_Command **)realloc(pipeline->commandv, new_size);
        if (pipeline->commandv == NULL) {
                mysh_perror("realloc");
                exit(EXIT_FAILURE);
        }
        pipeline->commandv[pipeline->commandc - 1] = command;
        pipeline->commandv[pipeline->commandc] = NULL;
}
