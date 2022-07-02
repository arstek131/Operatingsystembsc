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
	int nw,nr;
	pipe_t *piped;
	int i,j,k,u;
	int *pid;
	char numero[255];
	char ch;
	long int somma=0;

	//Controllo numero di parametri
	if(argc < 3)
	{
		printf("Numero di parametri < 2\n");
		exit(1);
	}

	int N=argc-1;

	//Alloco l'array di pipe per la comunicazione Padre-Figlio
	piped = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(piped == NULL)
	{
		printf("Allocazione array di pipe fallita\n");
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

	//Creo l'array di pid
	pid =(int *)malloc(N*sizeof(int));
	if(pid == NULL)
	{
		printf("Allocazione array di pid fallita\n");
		exit(4);
	}

	//Creo gli M figli
	for(i=0; i<N; i++)
	{
		pid[i]=fork();
		if(pid[i]<0)
		{
			printf("Chiamata a funzione fork() fallita\n");
			exit(5);
		}
		//caso del figlio
		else if(pid[i] == 0)
		{
			printf("Sono il figlio di indice %d e pid %d e sono partito\n",i,getpid());

			//Chiudo le pipe che non uso
			//Siccome i figli hanno solo il compito di scrivere al padre
			//In LETTURA chiudo tutto
			//In SCRITTURA lascio aperta la i-esima pipe
			for(j=0; j<N; j++)
			{
				close(piped[j][0]);
				if(i != j)
					close(piped[j][1]);
			}

			//Creo la pipe di comunicazione tra figlio e nipote
			pipe_t new;
			if(pipe(new)<0)
			{
				printf("Creazione della pipe Figlio-Nipote fallita\n");
				exit(-1);
			}

			//Creo il nipote
			pid[i]=fork();
			if(pid[i] < 0)
			{
				printf("Chiamata a funzione fork() fallita da parte del nipote\n");
				exit(-1);
			}
			//caso del nipote
			if(pid[i] == 0)
			{
				printf("Sono il processo nipote di pid %d e sono partito\n",getpid());

				//Chiudo la pipe di comunicazione lasciata aperta tra Padre-Figlio
				close(piped[i][1]);

				//chiudo la stdin
				close(0);

				//Apro il file di competenza, in questo modo il processo stesso
				//diventa stdin
				if(open(argv[i+1],O_RDONLY) < 0)
				{
					printf("Errore apertura file %s\n",argv[i+1]);
					exit(-1);
				}

				//Ora chiudo lo stdout, perchè la exec di default scrive su stdout
				//perchè io il messaggio non lo voglio su schermo ma devo buttarlo
				//sulla pipe per la comunicazione tra nipote e figlio
				close(1);
				dup(new[1]);

				//Ora ogni nipote può chiudere entrambi i lati della pipe
				//Il lato 0 non viene usato
				//il lato 1 viene usato da stdout
				close(new[0]);
				close(new[1]);

				//Ridirezione anche lo stderror su /dev/null (per evitare messaggi di errore a video)
				close(2);
				open(" /dev/null", O_WRONLY);

				//Quindi ora praticmante il nipote diventa il comando tail -1
				execlp("wc", "wc", "-l", (char *)0);

				exit(-5);



			}
			else
			{
				//Sono fuori dal nipote e torno nel figlio

				//Siccome il figlio non deve scrivere nulla..
				//Chiudo la pipe in SCRITTURA tra Figlio-Nipote
				close(new[1]);

				//Leggo dalla pipe il numero..
				u=0;
				while(read(new[0],&numero[u],1)==1)
				{
					u++;
				}

				if(u!=0)
				{
					numero[u-1]=0;
					k=atoi(numero);
				}
				else
				{
					k=0;
				}



				//printf("****STAMPA DEBUG %d %s *****\n",k,argv[i+1]);
				//Ora il figlio comunica al padre la lunghezza..
				nw = write(piped[i][1],&k,sizeof(int));
				if(nw != sizeof(int))
				{
					printf("Scrittura da parte del figlio di pid %d fallita sulla pipe\n",getpid());
					exit(-1);
				}


				//Il Figlio aspetta il nipote
				wait(NULL);
				exit(0);
			}
			
		}
	}
	//caso del padre

	printf("Sono il padre di pid %d e sono partito\n",getpid());

	//Chiudo le pipe che non uso
	//Siccome il padre ha solo il compito di LEGGERE dai figli
	//In SCRITTURA chiudo tutto
	//In LETTURA lascio inalterato

	for(i=0; i<N; i++)
	{
		close(piped[i][1]);
	}

	//Il padre legge dai figli in ordine inverso
	for(i=0; i<N; i++)
	{
		nr = read(piped[i][0],&k,sizeof(k));
		if(nr != sizeof(k))
		{
			printf("Lettura fallita dalla pipe da parte del padre\n");
			exit(-1);
		}
		somma=somma+(long int)k;
	}

	printf("La somma risultante derivante dai valori comunicati dai figli è %ld\n", somma); 

	//Il padre aspetta i figli
	for(i=0; i<N; i++)
	{
		wait(NULL);
	}

	printf("Sono il padre di pid %d ed e' terminato tutto correttamente\n",getpid());

	exit(0);



	
}