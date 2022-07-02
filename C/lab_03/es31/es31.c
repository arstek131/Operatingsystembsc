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
    int p[2];

    if(pipe(p)<0)
    {
        zprintf(2,"Errore invocazione pipe()\n");
        exit(EXIT_FAILURE);
    }

    zprintf(1,"p[0] (parte lettura)= %d\n",p[0]);
    zprintf(1,"p[1] (parte scrittura)= %d\n",p[1]);

    exit(EXIT_SUCCESS);
}



