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
    int n=0;
    int pip[2];
    char c='x';

    if(pipe(pip)<0)
    {
        zprintf(2,"Errore creazione pipe()\n");
        exit(EXIT_FAILURE);
    }

    while(write(pip[1],&c,1)>0)
    {
        zprintf(1,"Ho scritto %d byte.\n",++n);
    }

    exit(EXIT_SUCCESS);


}    
