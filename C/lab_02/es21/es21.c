#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdarg.h>



/*
 * Formatted write() funtion. Useful for writing
 * compact low-lowel code internally using write()
 * Example: zprintf(1, "argc = %d\n", argc);
 */
void zprintf(int fd, const char *fmt, ...) {
    static char msg[1024];
    int n;
    va_list ap;

    va_start(ap, fmt);
    n = vsnprintf(msg, 1024, fmt, ap);
    n = write(fd, msg, n);
    va_end(ap);
}


int main(int argc, char **argv)
{
    pid_t pid;

    pid=fork();

    switch(pid)
    {
        case 0: //significa che è il figlio
            zprintf(1," %d Ciao sono il figlio fork()=%d\n", getpid(), pid);
            exit(EXIT_SUCCESS);
        
        default: //signifca che è il padre
            zprintf(1," %d Ciao sono il padre fork()=%d\n", getpid(),pid);
            exit(EXIT_SUCCESS);
    }
}

























