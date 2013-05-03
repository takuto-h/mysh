#ifndef MYSH_MYSH_H_INCLUDED
#define MYSH_MYSH_H_INCLUDED

#define MYSH_LINE_BUFF_SIZE 1024
#define MYSH_WORD_BUFF_SIZE 256

typedef enum {
        MYSH_SUCCESS,
        MYSH_FAILURE
} Mysh_Result;

#include "mysh/pipeline.h"
#include "mysh/lexer.h"
#include "mysh/parser.h"
#include "mysh/runner.h"

void mysh_display(const char *format, ...);
void mysh_perror(const char *format, ...);

#endif /* MYSH_MYSH_H_INCLUDED */
