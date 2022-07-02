#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

typedef int pipe_t[2];

int main(int argc, char const *argv[])
{
	int nFigli;
	pipe_t *p;
	int i,j;
	int pid;

	//Controllo numero di argomenti
	if(argc != 2)
	{
		printf("Numero di argomenti sbagliato\n");
		exit(EXIT_FAILURE);
	}

	nFigli=atoi(argv[1]);
	if(nFigli <= 0)
	{
		printf("Solo numeri >0\n");
		exit(EXIT_FAILURE);
	}

	p = (pipe_t *) malloc(nFigli*sizeof(pipe_t));
	if(p == NULL)
	{
		printf("Allocazione dell'array di pipe fallita\n");
		exit(EXIT_FAILURE);
	}

	//Inizializzo le pipe
	for(i=0; i<nFigli; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Invocazione della chiamata fork() fallita\n");
			exit(EXIT_FAILURE);
		}
		//caso del figlio
		else if(pid == 0)
		{
			char cmd='l';
			printf("Sono il %d figlio con pid %d e ho cominciato\n",i,getpid());

			//Chiudo le pipe che non uso..
			//In lettura chiudo tutto tranne la prossima
			//In scrittura chiudo tutto tranne la prossima
			for(j=0; j<nFigli; j++)
			{
				if((j != i-1) && (i != 0))
					close(p[j][1]);
				if(j != i)
					close(p[j][0]);
			}

			printf("Sono il %d figlio con pid %d e ho ricevuto il carattere %c\n",i,getpid(),cmd);

			//Scrivo ora al figlio precedente
			//Se non sono più il figlio zero..
			if(i != 0)
			{
				cmd+=1;
				if((write(p[i-1][1],&cmd,sizeof(char))!=sizeof(char)))
				{
					printf("La scrittura da parte del %d figlio e' fallita\n",i);
					exit(EXIT_FAILURE);
				}
			}
			exit(EXIT_SUCCESS);
		}
	}
	//caso del padre
	if(pid > 0)
	{
		char cmd='k';

		printf("Sono il padre con pid %d e ho cominciato\n",getpid());

		//Chiudo le pipe che non uso..
		//Il padre ha solo il compito di leggere, quindi in lettura chiudo tutte le pipe
		//Per quanto riguarda la scrittura chiudo tutto tranne quella al prossimo figlio
		// siccome però è inversa, parto dalla fine..
		for(i=0; i<nFigli; i++)
		{
			if(i!=nFigli-1)
				close(p[i][1]);
			close(p[i][0]);
		}

		//Ora il padre scrive al figlio (all'inverso..)
		if(write(p[nFigli-1][1],&cmd,sizeof(char))!=sizeof(char))
		{
			printf("Scrittura da parte del padre fallita\n");
			exit(EXIT_FAILURE);
		}

		//Aspetto i figli
		for(i=0; i<nFigli; i++)
		{
			wait(NULL);
		}

		exit(EXIT_SUCCESS);

	}







}