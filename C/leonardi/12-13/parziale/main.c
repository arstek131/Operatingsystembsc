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
	pipe_t *piped;
	int i,j;
	int pid;
	char ch;

	//Controllo il numero di argomenti
	if(argc < 2)
	{
		printf("Numero di argomenti sbagliato\n");
		exit(1);
	}

	int nFigli=argc-1; 

	//Alloco l'array di N pipe per la comunicazione Padre-Figlio
	piped= (pipe_t *)malloc(nFigli*sizeof(pipe_t));

	if(piped == NULL)
	{
		printf("Allocazione dell'array di pipe fallita\n");
		exit(2);
	}

	//Creo effettivamente le pipe invocando la funzione pipe() sull'array
	for(i=0; i<nFigli; i++)
	{
		if(pipe(piped[i]) < 0)
		{
			printf("Errore nella chiamata a funzione pipe()\n");
			exit(3);
		}
	}

	//Ciclo di generazione dei figli
	for(i=0; i<nFigli; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Errore nella chiamata a funzione fork()\n");
			exit(4);
		}
		//caso del figlio
		if(pid == 0)
		{
			printf("Sono il %d figlio con pid %d e sono partito..\n",i,getpid());

			//Siccome devo andare ad usare un comando UNIX/Linux
			//	chiudo la stdout per poter poi usare la exec e poi la dup
			close(1);
			dup(piped[i][1]);

			//Chiudo tutte le pipe
			for(j=0; j<nFigli; j++)
			{
				close(piped[j][0]);
				close(piped[j][1]);
			}

			//Ridirigo lo stderror su /dev/null (per evitare messaggi di errore a video)
			close(2);
			open(" /dev/null", O_WRONLY);

			//Il figlio diventa il comando head
			execlp("head", "head", "-1", argv[i+1], (char *)0);

			exit(-1);
			


		}

	}
	//caso del padre
	if(pid > 0)
	{
		printf("Sono il padre di pid %d e sono partito..\n",getpid());

		//Chiudo le pipe che non uso..
		//Siccome il padre ha solo il compito di leggere
		//	in scrittura chiudo tutto..
		for(i=0; i<nFigli; i++)
		{
			close(piped[i][1]);
		}

		//Il padre ora recupera le informazioni dei figli in ordine.. 
		for(i=0; i<nFigli; i++)
		{
			printf("Il figlio di indice %d ha letto dal file %s la linea:\n",i,argv[i+1]);
			do
			{
				read(piped[i][0],&ch,1);
				printf("%c",ch);
			} while (ch != '\n');

		}

		//Il padre aspetta i figli
		for(i=0; i<nFigli; i++)
		{
			wait(NULL);
		}

		exit(0);
	}


















}