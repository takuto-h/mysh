#include <stdio.h>
#include <stdarg.h>
#include "mysh/mysh.h"

void
mysh_display(const char *format, ...)
{
        va_list ap;
        char buff[MYSH_LINE_BUFF_SIZE];

        va_start(ap, format);
        snprintf(buff, MYSH_LINE_BUFF_SIZE, "mysh: %s", format);
        vfprintf(stderr, buff, ap);
        va_end(ap);
}

void
mysh_perror(const char *format, ...)
{
        va_list ap;
        char buff1[MYSH_LINE_BUFF_SIZE];
        char buff2[MYSH_LINE_BUFF_SIZE];

        va_start(ap, format);
        snprintf(buff1, MYSH_LINE_BUFF_SIZE, "mysh: %s", format);
        vsnprintf(buff2, MYSH_LINE_BUFF_SIZE, buff1, ap);
        perror(buff2);
        va_end(ap);
}
