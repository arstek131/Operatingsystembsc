#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

typedef int pipe_t[2];

int main(int argc, char **argv)
{
	int pid;
	int i,j;
	int nFigli;
	pipe_t *p;

	//controllo il numero di argomenti
	if(argc != 2)
	{
		printf("Numero dia argomenti sbagliato\n");
		exit(EXIT_FAILURE);
	}

	nFigli=atoi(argv[1]);
	if(nFigli <= 0)
	{
		printf("Inserire solo numeri > 0\n");
		exit(EXIT_FAILURE);
	}

	//Alloco memoria per le pipe
	p=(pipe_t *) malloc(sizeof(pipe_t) * (nFigli));
	if(p == NULL)
	{
		printf("Allocazione dell'array di pipe fallito\n");
		exit(EXIT_FAILURE);
	}

	//Inizializzo le pipe
	for(i=0; i<nFigli; i++)
	{
		if(pipe(p[i])<0)
		{
			printf("Errore nella chiamata a funzione pipe()\n");
			exit(EXIT_FAILURE);
		}
	}

	//Inizializzo i figli
	for(i=0; i<nFigli; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Errore nella chiamata a funzione fork()\n");
			exit(EXIT_FAILURE);
		}
		//caso del figlio
		else if(pid == 0)
		{
			char cmd='a';

			printf("Sono il figlio %d con pid %d e ho cominciato..\n",i,getpid());

			//Chiudo le pipe non usate ovvero..
			//In scrittura il tutto parte dal primo figlio che scrive al prossimo
			// quindi chiudo tutto tranne la pipe attuale..
			//In lettura

			for(j=0; j<nFigli; j++)
			{
				if(j != i)
					close(p[j][1]);
				if((j != i-1) && (i != 0))
					close(p[j][0]);
			}

			//Il primo figlio deve attivare la pipeline
			if(i==0)
			{
				printf("Sono il figlio di indice %d e pid %d e sto per scrivere nella pipe\n",i,getpid());
				if(write(p[0][1],&cmd,sizeof(char))!=sizeof(char))
				{
					printf("La scrittura su pipe e' fallita\n");
					exit(EXIT_FAILURE);
				}
				exit(EXIT_SUCCESS);
			}

			//Ora il secondo figlio legge..
			if(read(p[i-1][0],&cmd,sizeof(char))!=sizeof(char))
			{
				printf("La lettura da parte del %d figlio e' fallita\n",i);
				exit(EXIT_FAILURE);
			}

			printf("Sono il %d figlio con pid %d e ho ricevuto il carattere %c\n",i,getpid(),cmd);

			cmd+=1;

			//Scrivo ora in avanti
			if(write(p[i][1],&cmd,sizeof(char))!=sizeof(char))
			{
				printf("Scrittura da parte del %d figlio con pid %d e' fallita\n",i,getpid());
				exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);
		}
	}
	//caso del padre
	if(pid > 0)
	{
		char cmd;

		printf("Sono il padre con pid %d e ho cominciato\n",getpid());

		//Siccome si tratta di una pipe aperta
		//Il padre  deve leggere solo dall'ultimo figlio, quindi in lettura chiudo tutto tranne l'ultimo
		//Mentre per quanto riguarda la scrittura chiudo tutto, siccome il padre non deve scrivere nulla
		for(i=0; i<nFigli; i++)
		{
			close(p[i][1]);
			if(i != nFigli-1)
				close(p[i][0]); //Lascio aperta in lettura l'ultima
		}

		//Leggo dall'ultimo figlio
		if(read(p[nFigli-1][0],&cmd,sizeof(char))!=sizeof(char))
		{
			printf("Errore nella lettura da parte del padre\n");
			exit(EXIT_FAILURE);
		}

		printf("Sono il padre e ho appena letto l'ultimo carattere %c dal figlio\n",cmd);

		//Aspetto i figli
		for(i=0; i<nFigli; i++)
		{
			wait(NULL);
		}

		exit(EXIT_SUCCESS);

	}




}