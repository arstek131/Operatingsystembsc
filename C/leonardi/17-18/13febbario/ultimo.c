#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

/* definisco un tipo per un array di due interi */
typedef int pipe_t[2];

typedef struct 
{
	long int c1;
	int c2;	
}s_occ;

int main(int argc, char const *argv[])
{
	pipe_t *piped;
	int i,j;
	int *pid;
	s_occ tmp;
	long int lun=0;
	int fd;
	char ch;
	int nw,nr;

	//Contorllo il numero di parametri
	if(argc < 2)
	{
		printf("Numero di parametri sbagliato\n");
		exit(1);
	}

	int M = argc-1;

	piped = (pipe_t *)malloc(M*sizeof(pipe_t));
	if(piped == NULL)
	{
		printf("Errore allocazione array di pipe\n");
		exit(2);
	}

	//Creo le pipe
	for(i=0; i<M; i++)
	{
		if(pipe(piped[i])<0)
		{
			printf("Errore chiamata a funzione pipe()\n");
			exit(3);
		}
	}

	//Creo l'array di pid
	pid=(int *)malloc(M*sizeof(int));
	if(pid == NULL)
	{
		printf("Errore allocazione array di pid\n");
		exit(4);
	}

	//Creo i figli
	for(i=0; i<M; i++)
	{
		pid[i]=fork();
		if(pid[i]<0)
		{
			printf("Errore chiamata a funzione fork()\n");
			exit(5);
		}
		//caso del figlio
		if(pid[i] == 0)
		{
			printf("Sono il figlio di indice %d e pid %d e sono partito\n",i,getpid());

			//Chiudo le pipe che non uso..
			//Siccome il primo figlio deve attivare la pipeline
			//In SCRITTURA lascio aperta la i-esima pipe mentre le altre le chiudo
			//In LETTURA se siamo al primo giro, chiudo tutto
			//	se siamo al 2 giro o più lascio aperta quella dietro e chiudo le altre

			for(j=0; j<M; j++)
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

			//Se si tratta del primo figlio
			if(i==0)
			{
				fd=open(argv[i+1],O_RDONLY);
				if(fd<0)
				{
					printf("Errore apertura file %s\n",argv[i+1]);
					exit(-1);
				}

				lun=0;
				while(read(fd,&ch,1)==1)
				{
					if(ch == '\n')
					{
						lun++;
					}
				}
				tmp.c1=lun;
				tmp.c2=0;

				//Scrivo al 2 figlio la struttura
				nw=write(piped[0][1],&tmp,sizeof(tmp));
				if(nw != sizeof(tmp))
				{
					printf("Errore scrittura del 1 figlio sulla pipe\n");
					exit(-1);
				}

			}
			else
			{
				s_occ tmp2;

				//Sono dal 2 figlio in poi
				fd=open(argv[i+1],O_RDONLY);
				if(fd<0)
				{
					printf("Errore apertura file %s\n",argv[i+1]);
					exit(-1);
				}

			    lun=0;
				while(read(fd,&ch,1)==1)
				{
					if(ch == '\n')
					{
						lun++;
					}
				}

				//Lego dal i-esimo figlio la struttura
				nr=read(piped[i-1][0],&tmp2,sizeof(tmp2));
				if(nr != sizeof(tmp2))
				{
					printf("Errore lettura da pipe da parte del %d figlio\n",i);
					exit(-1);
				}

				//Controllo il massimo
				if(lun > tmp2.c1)
				{
					tmp2.c1=lun;
					tmp2.c2=i;
				}

				//Comunico con una write la struttura al prossimo figlio
				nw = write(piped[i][1],&tmp2,sizeof(tmp2));
				if(nw != sizeof(tmp2))
				{
					printf("Errore scrittura su pipe da parte del %d figlio\n",i);
					exit(-1);
				}

			}

			exit(0);

		}
	}
	//caso del padre
	printf("Sono il padre di pid %d e sono partito\n",getpid());

	//Chiudo le pipe che non uso..
	//Siccome il padre ha solo il compito di leggere
	//In SCRITTURA chiudo tutto
	//In LETTURA lascio aperta l'ultima pipe

	for(i=0; i<M; i++)
	{
		close(piped[i][1]);
			if(i!=M-1)
				close(piped[i][0]);
	}

	//Leggo dall'ultimo figlio la struttura
	nr=read(piped[M-1][0],&tmp,sizeof(tmp));
	if(nr != sizeof(tmp))
	{
		printf("Errore lettura dalla pipe da parte del padre\n");
		exit(-1);
	}

	printf("Il pid del processo che ha calcolato il max e' %d con linee %ld dal figlio di indice %d per il file %s\n", pid[tmp.c2],tmp.c1,tmp.c2,argv[tmp.c2+1]);



	//Il padre aspetta i figli
	for(i=0; i<M; i++)
	{
		wait(NULL);
	}




	exit(0);
	
}