#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/* definisco un tipo per un array di due interi */
typedef int pipe_t[2];

int main(int argc, char const *argv[])
{
	int fd;
	int pid;
	pipe_t *piped;
	int i,j;
	char buffer[255];
	int ritorno,status;
	int pidFiglio;

	//Controllo il numero di parametri
	if(argc < 4)
	{
		printf("Numero di parametri sbagliato\n");
		exit(1);
	}


	int K = atoi(argv[argc-1]);

	if(K < 0)
	{
		printf("Numero negativo\n");
		exit(2);
	}

	int N = argc-2;

	int X;
	printf("Inserisci il numero X compreso tra 0 e %d\n",K);
	scanf("%d",&X);
	if(X < 0 || X > K)
	{
		printf("Numero non conforme alle specifiche\n");
		exit(3);
	}

	//Creo l'array di pipe per la comunicazione tra Padre-Figlio
	piped = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(piped == NULL)
	{
		printf("Allocazione array di pipe fallito\n");
		exit(4);
	}

	//Creo effetivamente le pipe
	for(i=0; i<N; i++)
	{
		if(pipe(piped[i]) < 0)
		{
			printf("Chiamata a funzione pipe fallita\n");
			exit(5);
		}
	}

	//Comincio la generazione dei figli
	for(i=0; i<N; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Chiamata a funzione fork() fallita\n");
			exit(6);
		}
		//Caso del figlio
		if(pid == 0)
		{
			printf("Sono il %d-figlio di pid %d e sono partito\n",i,getpid());

			//Chiudo le pipe che non uso
			//In LETTURA chiudo tutto, siccome i figli non devono leggere nulla
			//In SCRITTURA chiudo tutto tranne la i-esima pipe, per la comunicazione col padre
			for(j=0; j<N; j++)
			{
				close(piped[j][0]);
				if(j != i)
					close(piped[j][1]);
			}

			//Andiamo a leggere la X-esima linea
			fd=open(argv[i+1],O_RDONLY);
			if(fd < 0)
			{
				printf("Apertura del file fallita da parte del %d-figlio\n",i);
				exit(-1);
			}

			int trovata=0;
			int k=1; //Numero di linea
			int l=0;
			while(read(fd,&buffer[l],1)==1)
			{
				if(buffer[l] == '\n')
				{
					if(k == X)
					{
						l++;
						trovata=1;
						break;
					}
					else
					{
						l=0;
						k++;
					}
				}
				else
				{
					l++;
				}
			}

			if(trovata == 0) //significa che non è stata trovata la linea
			{
				sprintf(buffer,"Per il file %s non e' stata trovata la linea",argv[i+1]);
				l=strlen(buffer);
				ritorno=0;
			}
			else
			{
				buffer[l-1]=0;
				ritorno=l;
			}

			//Comunichiamo al padre la lunghezza
			write(piped[i][1],&l,sizeof(l));
			//Comunichaimo al padre la stringa
			write(piped[i][1],buffer,l);


			
			exit(ritorno);


		}
	}
	//Caso del padre
	if(pid > 0)
	{
		int lun;
		int nr;
		printf("Sono il padre di pid %d e sono partito..\n",getpid());
		//Chiudo le pipe che non uso
		//Siccome il padre ha solo il compito di leggere dai figli
		//In SCRITTURA chiudo tutto
		for(i=0; i<N; i++)
		{
			close(piped[i][1]);
		}

		//Leggiamo ciò che ci arriva dal padre
		for(i=0; i<N; i++)
		{
			nr=read(piped[i][0],&lun,sizeof(int));
			if(nr != sizeof(int))
			{
				printf("Il figlio con indice %d ha avuto problemi\n",i);
			}
			else
			{
				read(piped[i][0],buffer,lun);
				//buffer[lun-1]=0;
				printf("Il figlio di indice %d mi ha comunicato: %s\n",i,buffer);
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
				ritorno=(int)((status >> 8) & 0xFF);
				printf("Il figlio con pid %d ha ritornato %d\n",pidFiglio,ritorno);
			}
			
		}

		printf("Sono il padre di pid %d ed e' terminato tutto corretamente\n",getpid());
		exit(0);

	}
	










}