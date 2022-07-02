#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

typedef int pipe_t[2];

int main(int argc, char const *argv[])
{
	int pid;
	int i,j;
	pipe_t *piped;
	int nr,nw;

	//Controllo numero di parametri
	if(argc < 2)
	{
		printf("Numero di parametri sbagliato\n");
		exit(1);
	}

	int N = argc - 1;

	//Creo l'array di pipe per la comunicazione Padre-Figlio
	piped = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(piped == NULL)
	{
		printf("Allocazione array di pipe fallita\n");
		exit(2);
	}

	//Creo le pipe
	for(i=0; i<N; i++)
	{
		if(pipe(piped[i])<0)
		{
			printf("Chiamata a funzione pipe() fallita\n");
			exit(3);
		}
	}

	//Creo i figli
	for(i=0; i<N; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Chiamata a funzione pid() fallita\n");
			exit(4);
		}
		if(pid == 0)
		{
			printf("Sono il figlio di indice %d e pid %d e sono partito..\n",i,getpid());

			//chiudo la stdin
			close(0);

			//Apro il file di competenza, in questo modo il processo stesso
			//diventa stdin
			if(open(argv[i+1],O_RDONLY) < 0)
			{
				printf("Errore apertura file %s\n",argv[i+1]);
				exit(-1);
			}

			//Ora chiudo lo stdout, perchè la exec di default scrive su stdout
			//perchè io il messaggio non lo voglio su schermo ma devo buttarlo
			//sulla pipe per la comunicazione tra nipote e figlio
			close(1);
			dup(piped[i][1]);

			//Chiudo tutto le pipe
			for(j=0; j<N; j++)
			{
				close(piped[j][0]);
				close(piped[i][0]);
			}

			//Ridirezione anche lo stderror su /dev/null (per evitare messaggi di errore a video)
			close(2);
			open(" /dev/null", O_WRONLY);

			execlp("head", "head", "-1", (char *)0);

			exit(-1);
		}
	}
	//Caso del padre
	printf("Sono il padre di pid %d e sono partito..\n",getpid());

	//Chiudo le pipe che non uso..
	//Siccome il padre ha solo il compito di LEGGERE dai figli
	//In SCRITTURA chiudo tutto
	//In LETTURA lascio invariato
	for(i=0; i<N; i++)
	{
		close(piped[i][1]);
	}

	
	char ch;
	//Leggo in ordine dai figli
	for(i=0; i<N; i++)
	{
		//Versione 1 con bug file vuoto
		/*char myArr[255];
		j=0;
		int lun=0;
		while(read(piped[i][0],&myArr[j],1)==1)
		{
			j++;
			lun++;
		}
		myArr[lun-1]=0;
		printf("Il figlio di indice %d per il file %s ha letto %s\n",i,argv[i+1],myArr);
		fflush(stdout);*/

		//Versione due con letta carattere per carattere
		printf("Il figlio di indice %d per il file %s ha letto: \n",i,argv[i+1]);
		while(read(piped[i][0],&ch,1)==1)
		{
			if(ch == '\n')
				break;
			else
				printf("%c", ch);
		}
		printf("\n");
	}



	//Il padre aspetta gli N figli
	for(i=0; i<N; i++)
	{
		wait(NULL);
	}

	printf("Sono il padre di pid %d ed e' terminato tutto correttamente..\n",getpid());
	exit(0);












}