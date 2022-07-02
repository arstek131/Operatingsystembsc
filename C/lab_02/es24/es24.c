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

/*
 * Wait for a child an handles various exit cases.
 * Caution! This function terminates the execution
 * in case of errors in wait()
 */
void wait_child() {
    pid_t pid;
    int status;

    if ((pid = wait(&status)) == -1 ) {
        zprintf(1, "[%d] error: wait()", getpid());
        exit(EXIT_FAILURE);
    }
    if (WIFEXITED(status)) {
        zprintf(1, "[%d] child pid=%d exit=%d (normal)\n", getpid(), pid, WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status)) {
        zprintf(1, "[%d] child pid=%d signal=%d (terminated)\n", getpid(), pid, WTERMSIG(status));
    }
    else if (WIFSTOPPED(status)) {
        zprintf(1, "[%d] child pid=%d signal=%d (stopped)\n", getpid(), pid, WSTOPSIG(status));
    }
}

int child(int index) {
    zprintf(1, "[%d] Figlio %d iniziato...\n", getpid(), index);
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    int n=argc;
    int i;
    pid_t *pids; 
    pids=(pid_t *)malloc(sizeof(pid_t)*n); //creo l'array di pid

    for(i=0; i<n; i++)
    {
        pids[i]=fork();

        switch(pids[i])
        {
             case -1: //caso errore
                zprintf(2, "errore: fork()\n");
                exit(EXIT_FAILURE);
             case 0: //caso figlio
                child(i);
        }
    }


   //padre
    zprintf(1, "[%d] Il padre ha iniziato...\n", getpid());
    {
        wait_child();
    }
    exit(EXIT_SUCCESS);

}






