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

/*
#define SLOTS 16

int child(int *v, int *p)
{
    //v--> vettore di 16 interi
    //p--> vettore per la pipe

    int i;
    zprintf(1,"[%d] il figlio ha cominciato..\n",getpid());

    //chiudo la pipe in scrittura
    close(p[1]);

    for(i=0; i<SLOTS; i++)
    {
        //leggo dalla pipe
        if(read(p[0],&v[i],sizeof(int))!=sizeof(int))
        {
            zprintf(2,"Errore in read()\n");
            exit(EXIT_FAILURE);
        }

    }

    exit(EXIT_SUCCESS);
}

int father(int *v, int *p)
{
    int i;

    zprintf(1,"[%d] il padre ha cominciato..\n",getpid());

    //chiudo la pipe in lettura
    close(p[0]);
    // Initialize random numbers generator 
    srand(9786875L);

    sleep(1);

    for(i=0; i<SLOTS; i++)
    {
        //genero numero random
        v[i]=rand()%10;

        //scrivo sulla pipe
        if(write(p[1],&v[i],sizeof(int)!=sizeof(int)))
        {
            zprintf(2,"Errore nella write()\n");
            exit(EXIT_FAILURE);
        }
    }

    wait_child();
    exit(EXIT_SUCCESS);
}*/

/*
int main(int argc, char **argv)
{
    pid_t pid;
    int p[2];
    int v[SLOTS];

    //creo la pipe
    if(pipe(p)<0)
    {
        zprintf(2,"Errore creazione pipe()\n");
        exit(EXIT_FAILURE);
    }

    //faccio la fork per creare i due processi
    pid=fork();
    switch(pid)
    {
        case -1:
            zprintf(2,"Errore invocazione fork()\n");
            exit(EXIT_FAILURE);
        case 0: //caso del figlio
            child(v,p);
    }

    father(v,p);

    exit(EXIT_SUCCESS);


}*/

#define SLOTS 16

/* child function */
int child(int *v, int *p) {
    int i;

    zprintf(1, "[%d] child started...\n", getpid());

    /* close write-side pipe */
    close(p[1]);

    for (i = 0; i < SLOTS; i++) {
        /* read from pipe */
        if (read(p[0], &v[i], sizeof(int)) != sizeof(int)) {
            zprintf(1, "error: read()\n");
            exit(EXIT_FAILURE);
        }

        /* debug */
        zprintf(1, "[%d] (read) v[%d] = %d\n", getpid(), i, v[i]);
    }
    exit(EXIT_SUCCESS);
}

/* father function */
int father(int *v, int *p) {
    int i;

    zprintf(1, "[%d] father started...\n", getpid());

    /* close read-side pipe */
    close(p[0]);

    /* Initialize random numbers generator */
    srand(9786875L);

    sleep(1);

    for (i = 0; i < SLOTS; i++) {
        /* generate random number */
        v[i] = rand() % 10;

        /* write on pipe */
        if (write(p[1], &v[i], sizeof(int)) != sizeof(int)) {
            zprintf(1, "error: write()\n");
            exit(EXIT_FAILURE);
        }

        /* debug */
        zprintf(1, "[%d] (write) v[%d] = %d\n", getpid(), i, v[i]);
    }

    /* wait child before exit */
    wait_child();
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    pid_t pid;
    int p[2];
    int v[SLOTS];

    /* shared pipe */
    if (pipe(p) < 0) {
        zprintf(1, "error: pipe()\n");
        exit(EXIT_FAILURE);
    }

    /* fork process */
    pid = fork();
    switch (pid) {
        case -1: /* error */
            zprintf(1, "error: fork()\n");
            exit(EXIT_FAILURE);
        case 0: /* child */
            child(v, p);
    }
    father(v, p);
}

