#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

typedef int pipe_t[2];

typedef struct 
{
	long int c1;
	int c2;
	long int c3;
	
}s_occ;

int main(int argc, char const *argv[])
{
	long int counter=0;
	char ch;
	int fd;
	pipe_t *piped;
	int i,j;
	int *pid;
	s_occ tmp;
	int nr,nw;
	
	//Controllo numero di parametri
	if(argc < 4)
	{
		printf("Numero di parametri sbagliatp\n");
		exit(1);
	}

	if(strlen(argv[argc-1])>1)
	{
		printf("L'ultimo parametro non e' un singolo carattere\n");
		exit(2);
	}

	char Cx=argv[argc-1][0];
	int N=argc-2;

	//Creo l'array di pipe per la comunicazione tra i Figli e Padre
	piped = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(piped == NULL)
	{
		printf("Allocazione dell'array di pipe fallita\n");
		exit(3);
	}

	//Creo le pipe
	for(i=0; i<N; i++)
	{
		if(pipe(piped[i])<0)
		{
			printf("Chiamata a funzione pipe() fallita\n");
			exit(4);
		}
	}

	//Creo l'array di pid
	pid=(int *)malloc(N*sizeof(int));
	if(pid == NULL)
	{
		printf("Allocazione dell'array di pid fallita\n");
		exit(5);
	}

	//Creo gli N figli
	for(i=0; i<N; i++)
	{
		pid[i]=fork();
		if(pid[i] < 0)
		{
			printf("Chiamata a funzione pid() fallita\n");
			exit(6);
		}
		//Caso del figlio
		if(pid[i] == 0)
		{
			printf("Sono il figlio di indice %d e pid %d e sono partito..\n",i,getpid());

			//Chiudo le pipe che non uso..
			//Siccome il primo figlio deve attivare la pipeline
			//In SCRITTURA lascio aperta la i-esima pipe mentre le altre le chiudo
			//In LETTURA se siamo al primo giro, chiudo tutto
			//	se siamo al 2 giro o più lascio aperta quella dietro e chiudo le altre

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

			if(i==0) //Se si tratta del primo figlio
			{
				
				fd=open(argv[i+1],O_RDONLY);
				if(fd < 0)
				{
					printf("Apertura del %d file fallita\n",i);
					exit(-1);
				}

				counter=0;
				while(read(fd,&ch,1) == 1)
				{
					if(ch == Cx)
					{
						counter++;
					}
				}

				tmp.c1=counter;
				tmp.c2=0;
				tmp.c3=counter;

				nw = write(piped[0][1],&tmp,sizeof(tmp));
				if(nw != sizeof(tmp))
				{
					printf("Errore scrittura nella prima pipe\n");
					exit(-1);
				}

			}
			else
			{

				fd=open(argv[i+1],O_RDONLY);
				if(fd < 0)
				{
					printf("Apertura del %d file fallita\n",i);
					exit(-1);
				}

				counter=0;
				while(read(fd,&ch,1) == 1)
				{
					if(ch == Cx)
					{
						counter++;
					}
				}

				nr = read(piped[i-1][0],&tmp,sizeof(tmp));
				if(nr != sizeof(tmp))
				{
					printf("Lettura fallita da parte del %d figlio\n",i);
					exit(-1);
				}

				if(counter > tmp.c1)
				{
					tmp.c1=counter;
					tmp.c2=i;
				}

				tmp.c3=tmp.c3+counter;

				nw = write(piped[i][1],&tmp,sizeof(tmp));
				if(nw != sizeof(tmp))
				{
					printf("Scrittura sulla pipe da parte del %d figlio fallita\n",i);
					exit(-1);
				}


			}

			exit(0);

		}
	}
	//Caso del padre
	
		
		
		printf("Sono il padre di pid %d e sono partito..\n", getpid());

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

		//Il padre legge dall'ultima pipe..
		nr=read(piped[N-1][0],&tmp,sizeof(tmp));
		if(nr != sizeof(tmp))
		{
			printf("Lettura da parte del padre fallita\n");
			exit(-1);
		}

		printf("\nIl valore massimo di occorrenze e' %ld calcolato dal figlio di indice %d e pid %d per il file %s ,mentre la somma di tutte le occorrenze e' %ld\n",tmp.c1,tmp.c2,pid[tmp.c2],argv[tmp.c2+1],tmp.c3);



		//Il padre aspetta i figli
		for(i=0; i<N; i++)
		{
			wait(NULL);
		}

		printf("\nSono il padre di pid %d ed e' terminato tutto correttamente\n",getpid());
	















}