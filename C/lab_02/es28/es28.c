#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>

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

int read_bytes(char *nomeFile)
{
    int fd, n = 0;
    char buffer;

    /* Apro il file in lettura */
    fd = open(nomeFile, O_RDONLY);
    if (fd < 0) {
        zprintf(2, "errore: open()\n");
        exit(EXIT_FAILURE);
    }

    /* conto i caratteri */
    while (read(fd, &buffer, 1) > 0) n++;
    return n;

}

void child(int index, int argc, char **argv)
{
    zprintf(1, "[%d] Figlio %d iniziato...\n", getpid(), index);
    zprintf(1, "[%d] Il figlio ha contato %d bytes\n", getpid(), read_bytes(argv[1]));
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    pid_t pid;

    pid = fork();
    switch (pid)
    {
        case -1: /* error */
            zprintf(2, "error: fork()\n");
            exit(EXIT_FAILURE);
        case 0: /* child */
            child(0, argc, argv);
            exit(EXIT_SUCCESS);
    }
    
    //padre
    zprintf(1, "[%d] Father started...\n", getpid());  
    zprintf(1, "[%d] Father counted %d bytes\n", getpid(), read_bytes(argv[1]));
    wait_child();
    exit(EXIT_SUCCESS);

}





