#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

/* definisco un tipo per un array di due interi */
typedef int pipe_t[2];

int main(int argc, char const *argv[])
{
	pipe_t *pipe_fp, *pipe_pf;
	int i,j;
	int *pid;
	int fd;
	int posizione, occorrenze;
	char ch;
	int nw, nr;
	char chControllo;

	//Controllo il numero di parametri
	if(argc < 4)
	{
		printf("Numero di parametri sbagliato\n");
		exit(1);
	}

	if(strlen(argv[1])!=1)
	{
		printf("Il primo parametro non e' un singolo carattere\n");
		exit(2);
	}

	char CZ = argv[1][0];
	int N = argc-2;

	/* allocazione memoria dinamica per pipe_fp e pipe_pf */
	pipe_fp=malloc(N*sizeof(pipe_t));
	pipe_pf=malloc(N*sizeof(pipe_t));
	if ((pipe_fp == NULL) || (pipe_pf == NULL))
	{
		printf("Errore nelle malloc\n");
		exit(3);
	}

	/* creazione delle pipe */
	for (i=0; i < N; i++) 
	{
		if (pipe(pipe_fp[i])!=0) 
		{
			printf("Errore creazione delle pipe figli-padre\n");
			exit(4);
		}
		if (pipe(pipe_pf[i])!=0) 
		{
			printf("Errore creazione delle pipe padre-figli\n");
			exit(5);
		}
	}


	

	//Creo l'array di pid
	pid = (int *)malloc(N*sizeof(int));
	if(pid == NULL)
	{
		printf("Allocazione array di pid fallita\n");
		exit(6);
	}

	for(i=0; i<N; i++)
	{
		pid[i]=fork();
		if(pid[i]<0)
		{
			printf("Chiamata a funzione fork() fallita\n");
			exit(7);
		}
		//caso del figlio
		if(pid[i]==0)
		{
			printf("Sono il figlio di indice %d e pid %d e sono partito..\n",i,getpid());

			//Chiudo le pipe che non uso..
			//Ogni figlio scrive solo su pipe_fp[i] e legge solo da pipe_pf
			for (j=0;j<N;j++)
      		{
        		close(pipe_fp[j][0]);
        		close(pipe_pf[j][1]);
        		if (j!=i)
        		{
        			close(pipe_fp[j][1]);
        			close(pipe_pf[j][0]);
        		}
      		}

      		//Apro il file di competenza
      		fd=open(argv[i+2], O_RDONLY);
      		if(fd < 0)
      		{
      			printf("Impossibile aprire il file %s\n",argv[i+2]);
      			exit(-1);
      		}

      		posizione=0;
      		occorrenze=0;
      		while(read(fd,&ch,1)==1)
      		{
      			if(ch == Cz)
      			{
      				occorrenze++;
      				//Invio la posizione corrente al padre
      				nw = write(pipe_fp[i][1],&posizione,sizeof(posizione));
      				if(nw != sizeof(posizione))
      				{
      					printf("Impossibile scrivere sulla pipe per il processo di indice %d\n", i);
      					exit(-1);
      				}

      				//Aspetto dal padre se devo scrivere o meno
      				nr=read(pipe_pf[i][0],&chControllo,1);
      				if(nr!=sizeof(chControllo))
      				{
      					printf("Impossibile leggere dalla pipe per il processo di indice %d\n",i);
      					exit(-1);
      				}

      				if(chControllo == 'S')
      				{
      					printf("Sono il figlio di indice %d e pid %d e ho trovato una occorrenza del carattere %c alla posizione %ld nel file %s\n",i,getpid(),CZ,posizione,argv[i+2]);

      				}

      			}
      			posizione++;
      		}
      		exit(occorrenze);

		}
	}
	//Caso del padre
	printf("Sono il padre di pid %d e sono partito\n",getpid());

	//Chiudo le pipe che non uso
	for(i=0; i<N; i++)
	{
		close(pipe_fp[i][1]);
   		close(pipe_pf[i][0]);
	}

	












	exit(0);
	
	
}