#ifndef MYSH_PIPELINE_H_INCLUDED
#define MYSH_PIPELINE_H_INCLUDED

typedef struct {
        int src_fd;
        enum {
                MYSH_REDIR_FROM_FILE,
                MYSH_REDIR_TO_FILE,
                MYSH_REDIR_TO_FILE_APPEND,
                MYSH_REDIR_TO_FD
        } dst_type;
        union {
                char *file;
                int fd;
        } dst;
} Mysh_Redir;

typedef struct {
        char *file;
        char **argv;
        int argc;
        Mysh_Redir **redirv;
        int redirc;
} Mysh_Command;

typedef struct {
        Mysh_Command **commandv;
        int commandc;
} Mysh_Pipeline;

Mysh_Redir *mysh_redir_from_file_new(int src_fd, const char *dst_file);
Mysh_Redir *mysh_redir_to_file_new(int src_fd, const char *dst_file);
Mysh_Redir *mysh_redir_to_file_append_new(int src_fd, const char *dst_file);
Mysh_Redir *mysh_redir_to_fd_new(int src_fd, int dst_fd);
void mysh_redir_delete(Mysh_Redir *redir);

Mysh_Command *mysh_command_new(const char *file);
void mysh_command_delete(Mysh_Command *command);
void mysh_command_add_arg(Mysh_Command *command, const char *arg);
void mysh_command_add_redir(Mysh_Command *command, Mysh_Redir *redir);

Mysh_Pipeline *mysh_pipeline_new(Mysh_Command *command);
void mysh_pipeline_delete(Mysh_Pipeline *pipeline);
void mysh_pipeline_add(Mysh_Pipeline *pipeline, Mysh_Command *command);

#endif /* MYSH_PIPELINE_H_INCLUDED */
