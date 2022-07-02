#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

typedef int pipe_t[2];


int main(int argc, char **argv)
{
	int nr,nw;
	char car,sendCar; //carattere letto dal figlio
	int pid;
	int i,j;
	int nFigli;
	pipe_t *p; //variabile usata per crearmi l'array di pipe (una per ogni figlio)

	//Controllo il numero degli argomenti
	if(argc != 2)
	{
		printf("Numero di argomenti sbagliato\n");
		exit(EXIT_FAILURE);
	}

	//Mi salvo il numero di figli dalla linea di comando
	nFigli=atoi(argv[1]);
	if(nFigli <= 0)
	{
		printf("Inserire un numero > 0\n");
		exit(EXIT_FAILURE);
	}

	//Alloco memoria per le pipe, 1 per ogni figlio
	p = (pipe_t *) malloc(sizeof(pipe_t)*nFigli);
	if(p==NULL)
	{
		printf("Errore nell'allocazione dell'array di pipe\n");
		exit(EXIT_FAILURE);
	}

	//Inizializzo le pipe
	for(i=0; i<nFigli; i++)
	{
		if(pipe(p[i])<0)
		{
			printf("Errore nell'invocazione della funzione pipe()\n");
			exit(EXIT_FAILURE);
		}
	}

	//Inizializzo i processi (faccio un for che va ciclico)
	for(i=0; i<nFigli; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Errore nell'inovazione della funzione fork()\n");
			exit(EXIT_FAILURE);
		}
		//caso del figlio
		else if(pid == 0 )
		{
			printf("Il figlio %d ha cominciato..\n",getpid());

			//Chiudo la parte delle pipe che non mi serve
			//Il padre ha il compito di scrivere
			//Il figlio ha il compito di leggere
			//Quindi chiudo in scrittura tutto dalla parte dei figli
			//E chiudo anche tutto in lettura (per i figli) tranne per il figlio
			// preso in considerazione attualmente
			for(j=0; j<nFigli; j++)
			{
				close(p[j][1]);
				if(i != j) close(p[j][0]);
			}

			nr=read(p[i][0],&car,sizeof(char));
			if(nr != sizeof(char))
			{
				printf("Errore nella lettura del carattere da parte del %d figlio\n",i);
				exit(EXIT_FAILURE);
			}

			printf("Il figlio %d di indice %d ha letto con successo il carattere %c\n",getpid(),i,car);
			exit(EXIT_SUCCESS);


		}
	}
	//caso del padre
	if(pid > 0)
	{
		printf("Il padre %d ha cominciato..\n",getpid());
		sendCar='a';

		//chiudo la parte delle pipe che non mi serve..
		//Siccome il padre ha il compito di scrivere e basta
		//Allora chiudiamo tutte le letture, lasciando aperte le scritture

		for(i=0; i<nFigli; i++)
		{
			close(p[i][0]);
		}

		//Scrivo per ogni figlio il contenuto..
		for(i=0; i<nFigli; i++)
		{
			printf("Sto per scrivere nella pipe per il figlio %d\n",i);
			nw=write(p[i][1],&sendCar,sizeof(char));
			if(nw != sizeof(char))
			{
				printf("Errore nella scrittura su pipe da parte del padre\n");
				exit(EXIT_FAILURE);
			}
			sendCar+=1;
		}

		//Aspetto tutti i figli..
		for(i=0; i<nFigli; i++)
		{
			wait(NULL);
		}

		exit(EXIT_SUCCESS);

	}






}