#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

/* definisco un tipo per un array di due interi */
typedef int pipe_t[2];

int main(int argc, char const *argv[])
{
	int fd;
	pipe_t *newPip;
	pipe_t *piped;
	int *pid;
	int i,j,k;
	int nw,nr;
	char ch;
	long int counter=0;
	long int counterF=0;
	
	//Controllo il numero di parametri
	if(argc < 3)
	{
		printf("Numero di parametri sbagliato\n");
		exit(1);
	}

	int N= argc-1;

	//Creo l'array di pipe per la comunicazione Padre-Figlio
	piped = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(piped == NULL)
	{
		printf("Allocazione array di pipe fallita\n");
		exit(1);
	}

	//Creo le pipe di comunicazione Padre-Figlio
	for (i = 0; i < N; ++i)
	{
		if(pipe(piped[i])<0)
		{
			printf("Chiamata a funzione pipe() fallita\n");
			exit(2);
		}
	}

	//Creo l'array di pipe per la comunicazione Padre-Nipote
	newPip = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(newPip == NULL)
	{
		printf("Allocazione array di pipe fallita\n");
		exit(3);
	}

	//Creo le pipe di comunicazione Padre-Nipote
	for (i = 0; i < N; ++i)
	{
		if(pipe(newPip[i])<0)
		{
			printf("Chiamata a funzione pipe() fallita\n");
			exit(4);
		}
	}

	//Creo l'array di pid
	pid = (int *)malloc(N*sizeof(int));
	if(pid == NULL)
	{
		printf("Allocazione array di pid fallita\n");
		exit(5);
	}

	//Creo i figli
	for (i = 0; i < N; i++)
	{
		pid[i]=fork();
		if(pid[i]<0)
		{
			printf("Chiamata a funzione fork() fallita\n");
			exit(6);
		}
		//caso del figlio
		if(pid[i] == 0)
		{
			printf("Sono il figlio di indice %d e pid %d e sono partito\n",i,getpid());

			//Chiudo le pipe che non uso
			//Siccome il figlio non deve leggere nulla
			//In LETTURA chiudo tutto
			//In SCRITTURA lascio aperta la i-esima pipe
			for (j = 0; j < N; j++)
			{
				close(piped[j][0]);
				if(i != j)
					close(piped[j][1]);
				
			}

			//Genero il nipote..
			pid[i]=fork();
			if(pid[i]<0)
			{
				printf("Chiamata fork() fallita\n");
				exit(-1);
			}
			//Caso del nipote
			if(pid[i]==0)
			{
				printf("Sono il nipote di pid %d e sono partito\n",getpid());

				//Chiudo la pipe rimasta aperta da Padre-Figlio
				close(piped[i][1]);

				//Chiudo le pipe che non uso
				//Siccome il nipote non deve leggere nulla
				//In LETTURA chiudo tutto
				//In SCRITTURA lascio aperta la i-esima pipe
				for (j = 0; j< N; j++)
				{
					close(newPip[j][0]);
					if(i != j)
						close(newPip[j][1]);
					
				}



				fd=open(argv[i+1],O_RDWR);
				if(fd < 0)
				{
					printf("Apertura del file %s fallita dal nipote\n",argv[i+1]);
					exit(-1);
				}

				counter=0;
				while(read(fd,&ch,1)==1)
				{
					if(isalpha(ch))
					{
						if(islower(ch))
						{
							//Torno indietro di una posizione
							lseek(fd, -1L, SEEK_CUR);
							ch=toupper(ch);
							write(fd,&ch,1);
							counter++;
						}
					}
				}

				//Il nipote invia al padre il numero di trasformazioni
				nw=write(newPip[i][1],&counter,sizeof(counter));
				if(nw != sizeof(counter))
				{
					printf("Scrittura da parte del nipote fallita sulla pipe\n");
					exit(-1);
				}

				exit(0);

			}
			//Torno nel figlio
			//Le pipe usate dal nipote vanno chiuse TUTTE
			for (j=0; j < N; j++)
			{
				close(newPip[j][0]);
				close(newPip[j][1]);
			}

			fd=open(argv[i+1],O_RDWR);
			if(fd < 0)
			{
				printf("Apertura del file %s fallita dal figlio\n",argv[i+1]);
				exit(-1);
			}

			counterF=0;
			while(read(fd,&ch,1)==1)
			{
				if(isdigit(ch))
				{	
					//Torno indietro di una posizione
					lseek(fd, -1L, SEEK_CUR);
					ch=' ';
					write(fd,&ch,1);
					counterF++;
				}
			}

			//Il figlio invia al padre il numero di trasformazioni
			nw=write(piped[i][1],&counterF,sizeof(counterF));
			if(nw != sizeof(counterF))
			{
				printf("Scrittura da parte del figlio fallita sulla pipe\n");
				exit(-1);
			}

			//Il figlio aspetta il nipote
			wait(NULL);
			exit(0);

		}
		
	}
	//Caso del padre
	printf("Sono il padre di pid %d e sono partito\n",getpid());

	//Chiudo le pipe che non uso.. (Padre-Figlio)
	//Siccome il padre ha solo il compito di leggere dal figlio
	//In SCRITTURA chiudo tutto
	for (i = 0; i < N; ++i)
	{
		close(piped[i][1]);
	}

	//Chiudo le pipe che non uso.. (Padre-Nipote)
	//Siccome il padre ha solo il compito di leggere dal nipote
	//In SCRITTURA chiudo tutto
	for (i = 0; i < N; ++i)
	{
		close(newPip[i][1]);
	}

	long int tmp,tmp2;

	//Il padre recupera le informazioni dai nipoti e dai figli
	for (i = 0; i < N; ++i)
	{
		nr=read(piped[i][0],&tmp,sizeof(tmp));
		if(nr != sizeof(tmp))
		{
			printf("Lettura fallita da parte del padre per il %d figlio\n",i);
			exit(-1);
		}
		printf("Il figlio di indice %d ha operato %ld trasformazioni di caratteri numerici in carattere spazio sul file %s\n", i, tmp, argv[i+1]);

		nr=read(newPip[i][0],&tmp2,sizeof(tmp2));
		if(nr != sizeof(tmp2))
		{
			printf("Lettura fallita da parte del padre per il %d nipote\n",i);
			exit(-1);
		}
		printf("Il nipote di indice %d ha operato %ld trasformazioni di caratteri minuscoli in MAIUSCOLI sullo stesso file %s\n", i, tmp2, argv[i+1]);

	}


	//Il padre aspetta i figli
	for (i = 0; i < N; ++i)
	{
		wait(NULL);
	}


	exit(0);



}