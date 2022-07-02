#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

typedef int pipe_t[2];

int main(int argc, char **argv)
{
	int nw,nr;
	int pid;
	int i,j;
	int nFigli;
	pipe_t *p;

	//Faccio il controllo sul numero di argomenti
	if(argc != 2)
	{
		printf("Errore --> numero di argomenti sbaliato\n");
		exit(EXIT_FAILURE);
	}

	nFigli=atoi(argv[1]);
	if(nFigli <= 0)
	{
		printf("Per favore inserire solo numeri > 0\n");
		exit(EXIT_FAILURE);
	}

	//Alloco la memoria per N+1 pipes
	p = (pipe_t *) malloc(sizeof(pipe_t) * (nFigli + 1));
	if(p == NULL)
	{
		printf("Errore nell'allocazione dell'array di pipe\n");
		exit(EXIT_FAILURE);
	}

	//Inizializzo le pipe
	for(i=0; i<nFigli+1; i++)
	{
		if(pipe(p[i]) < 0)
		{
			printf("Errore nell'invocazione della chiamata a funzione pipe()\n");
			exit(EXIT_FAILURE);
		}
	}

	//Inizializzo gli N processi figli
	for(i=0; i<nFigli; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Errore nella chiamata a funzione fork()\n");
			exit(EXIT_FAILURE);
		}
		//caso del figlio
		else if (pid == 0)
		{
			char carSendRead;
			printf("Sono il %d figli con pid %d e ho cominciato...\n",i,getpid());

			//Chiudo le pipe che non mi servono..
			//In lettura chiudo tutto tranne la mia
			//In scrittura chiudo tutto tranne la prossima
			for(j=0; j<nFigli+1; j++)
			{
				if(j != i+1)
					close(p[j][1]);
				if(j != i)
					close(p[j][0]);
			}

			//Leggo..
			if(read(p[i][0],&carSendRead,sizeof(char))!=sizeof(char))
			{
				printf("Errore nella lettura da parte del figlio..\n");
				exit(EXIT_FAILURE);
			}

			printf("Sono il %d figlio con pid %d e ho ricevuto il carattere %c\n",i,getpid(),carSendRead);

			carSendRead+=1;

			//scrivo al prossimo figlio o padre
			if(write(p[i+1][1],&carSendRead,sizeof(char))!=sizeof(char))
			{
				printf("La scrittura da parte del %d con pid %d e' fallita..\n",i,getpid());
				exit(EXIT_FAILURE);
			}

			exit(EXIT_SUCCESS);

		}

	}
	//caso del padre
	if(pid > 0)
	{
		char carSendRead='a';
		//Il padre praticamente è quello che deve scrivere il primo carattere al prossimo figlio
		//Ed è anche quello che deve leggere dall'ultima pipe..
		printf("Sono il padre con pid %d e ho cominciato...\n",getpid());

		//Chiudo le pipe che non mi servono..
		//In scrittura chiudo tutte le pipe tranne quella del prossimo figlio dove voglio scrivere..
		//In lettura chiudo tutte tranne l'ultima.. perchè da li poi il padre deve leggere ciò
		//	che gli arriva poi dall'ultimo figlio
		for(i=0; i<nFigli+1; i++)
		{
			if(i!=0)
				close(p[i][1]);
			if(i!=nFigli)
				close(p[i][0]);
		}

		printf("Sono il padre con pid %d e sto per scrivere al 1 figlio in assoluto..\n",getpid());

		if(write(p[0][1],&carSendRead,sizeof(char)) != sizeof(char))
		{
			printf("Errore nella 1 scrittura da parte del padre\n");
			exit(EXIT_FAILURE);
		}

		//Ora devo leggere dall'ultimo figlio..
		if(read(p[nFigli][0],&carSendRead,sizeof(char)) != sizeof(char))
		{
			printf("Errore nell'ultima lettura (quella dove il padre deve leggere)\n");
			exit(EXIT_FAILURE);
		}

		printf("Sono il padre e ho ricevuto dall'ultimo figlio il carattere %c\n",carSendRead);

		//Aspetto i figli prima di uscire
		for(i=0; i<nFigli; i++)
		{
			wait(NULL);
		}

		exit(EXIT_SUCCESS);


	}







}