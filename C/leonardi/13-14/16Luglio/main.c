#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

typedef int pipe_t[2];

int global=0;

int main(int argc, char const *argv[])
{
	pipe_t *piped;
	int *pid;
	char myArr[255];
	int i,j;
	int fd;
	int nr,nw;
	char *primiCar;
	int k=0;
	char toSend;
	int pidFiglio;
	int status;
	char ritorno;

	//Controllo numero di parametri
	if(argc < 4)
	{
		printf("Numero di argomenti sbagliato\n");
		exit(1);
	}

	int X = atoi(argv[argc-1]);

	int N = argc-2;

	//Creo l'array di pipe per la comunicazione Padre-Figlio
	piped = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(piped == NULL)
	{
		printf("Allocazione arrayd di pipe fallita\n");
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

	//Creo l'array di caratteri da inviare in avanti tutte le volte
	primiCar=(char *)malloc(N*sizeof(char));
	if(primiCar == NULL)
	{
		printf("Allocazione array di char primiCar fallita\n");
		exit(4);
	}

	pid=(int *)malloc(N*sizeof(int));
	if(pid == NULL)
	{
		printf("Allocazione array di pid fallita\n");
		exit(5);
	}

	//Creo gli N figli
	for(i=0; i<N; i++)
	{
		//global=0;
		pid[i]=fork();
		if(pid[i]<0)
		{
			printf("Chiamata a funzione fork() fallita\n");
			exit(4);
		}
		//Caso del figlio
		if(pid[i] == 0)
		{
			printf("Sono il figlio di indice %d e pid %d e sono partito\n",i,getpid());

			//Chiudo le pipe che non uso..
			//Siccome il primo figlio deve attivare la pipeline
			//In SCRITTURA lascio aperta la i-esima pipe mentre le altre le chiudo
			//In LETTURA se siamo al primo giro, chiudo tutto
			//se siamo al 2 giro o più lascio aperta quella dietro e chiudo le altre

			for(j=0; j<N; j++)
			{
				if(i != j)
					close(piped[j][1]);
				if(i==0)
					close(piped[j][0]);
				else
				{
					if(i!=j+1)
						close(piped[j][0]);
				}
			}


				fd=open(argv[i+1],O_RDONLY);

				if(fd < 0)
				{
					printf("Apertura del file %s fallita\n",argv[i+1]);
					exit(-1);
				}

				k=0;
				while(read(fd,&myArr[k],1)==1)
				{
					if(myArr[k] == '\n')
					{
						//Se non sono il primo figlio allora devo leggere dalla pipe prima anche
						if(i!=0)
						{
							nr=read(piped[i-1][0],primiCar,N);
							if(nr != sizeof(char)*N)
							{
								printf("Lettura dell'array fallita dal %d figlio\n",i);
								exit(-1);
							}

						}
						primiCar[i]=myArr[0];
						nw=write(piped[i][1],primiCar,N);
						k=0;
					}
					else
					{
						k++;
					}	

				}
				exit(myArr[0]);
			}
	}
	//Caso del padre
	printf("Sono il padre di pid %d e sono partito\n",getpid());

	//Chiudo le pipe che non uso..
	//Siccome il padre ha solo il compito di leggere
	//In SCRITTURA chiudo tutto		
	//In LETTURA lascio aperta l'ultima pipe

	for(i=0; i<N; i++)
	{
		close(piped[i][1]);
		if(i!=N-1)
			close(piped[i][0]);
	}

	
	for(i=0; i<X; i++)
	{
		nr=read(piped[N-1][0],primiCar,N*sizeof(char));
		if(nr != sizeof(char)*N)
		{
			printf("Lettura fallita da parte del padre per la %d linea\n",i+1);
			exit(-1);
		}
		printf("Per la linea %d dai file e' stato letto..\n",i+1);
		for(j=0; j<N; j++)
		{
			printf("Il carattere %c dal file %s con pid del figlio %d\n",primiCar[j],argv[j+1],pid[j]);

		}
		

	}


	//Il padre aspetta i figli
	for(i=0; i<N; i++)
	{
		pidFiglio = wait(&status);
			if(pidFiglio < 0)
			{
				printf("Errore nella wait\n");
				exit(8);
			}
			if ((status & 0xFF) != 0)
			{
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
			}
			else
			{
				ritorno=(char)((status >> 8) & 0xFF);
				printf("Il figlio con pid %d ha ritornato %c\n",pidFiglio,ritorno);

			}
	}




	printf("Sono il padre di pid %d ed e' terminato tutto corretamente\n",getpid());
	exit(0);


	

}