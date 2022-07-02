#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>

typedef int pipe_t[2];

typedef struct 
{
	int c1;
	int c2;
	char c3[250];
	
}s_occ;


int mia_random(int n)
{
	int casuale;
	casuale = rand() % n;
	casuale++;
	return casuale;
}

int main(int argc, char const *argv[])
{
	int ritorno;
	int k;
	char opzione[5];
	int nr,nw;
	int pid;
	int i,j;
	pipe_t *piped;

	//Controllo numero di parametri
	if(argc < 3 || (argc-1)%2)
	{
		printf("Numero di parametri sbagliato\n");
		exit(1);
	}

	int N = argc-1;

	//Controllo che i parametri in posizione pari siano numeri > 0
	for(i=0; i<argc; i++)
	{
		if(i==0)
			continue;
		if(i%2==0)
		{
			int tmp=atoi(argv[i]);
			if(tmp <= 0)
			{
				printf("Numero negativo!\n");
				exit(2);
			}
		}
	}

	N=N/2;

	//Creo l'array di comunicazione per Padre-Figlio
	piped = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(piped == NULL)
	{
		printf("Allocazione array di pipe fallito\n");
		exit(3);
	}

	//Creo le pipe
	for(i=0; i<N; i++)
	{
		if(pipe(piped[i]) < 0)
		{
			printf("Chiamata a funzione pipe() dal padre fallita\n");
			exit(4);
		}
	}

	//Genero gli N figli
	for(i=0; i<N; i++)
	{
		pid = fork();
		if(pid < 0)
		{
			printf("Chiamata a funzione fork() fallita da parte del padre\n");
			exit(5);
		}
		//Caso del figlio
		if(pid == 0)
		{
			printf("Sono il %d figlio di pid %d e sono partito..\n",i,getpid());

			//Chiudo le pipe che non uso...
			//Siccome i figli nei confronti del padre hanno solo il compito di SCRIVERE
			//In LETTURA chiudo tutto
			//In SCRITTURA lascio aperta la i-esima pipe
			for(j=0; j<N; j++)
			{
				close(piped[j][0]);
				if(j != i)
					close(piped[j][1]);
			}

			//Creo la pipe di comunicazione tra Figlio-Nipote
			pipe_t new;
			if(pipe(new) < 0)
			{
				printf("Allocazione pipe Figlio-Nipote fallita\n");
				exit(6);
			}

			//Creo il nipote
			pid = fork();
			if(pid < 0)
			{
				printf("Chiamata fork() fallita per il nipote\n");
				exit(7);
			}
			//Sono nel nipote
			if(pid == 0)
			{
				printf("Sono il nipote di pid %d e ho cominciato..\n",getpid());
				//Chiudo la pipe di comunicazione rimasta aperta tra padre figlio
				close(piped[i][1]);
				
				srand(time(NULL));
				int value=atoi(argv[(i*2)+2]);;
				int toRead=mia_random(value);

				//Ogni nipote deve simulare il piping dei comandi nei confronti
				//	del figlio e quindi chiudere lo stdout e usare quindi la dup

				close(1);
				dup(new[1]);

				close(new[0]);
				close(new[1]);

				sprintf(opzione, "-%d", toRead);

				execlp("head", "head", opzione, argv[(i*2)+1], (char *)0);


				exit(i);
			}

			//Torno nel figlio

			//Siccome il figlio non deve scrivere nulla nel nipote
			//In SCRITTURA chiudo
			close(new[1]);

			/*Adesso il compito del figlio è leggere dalla pipe del Figlio-Nipote
			  finchè ci sono caratteri.. Siccome il nipote ha eseguito una execlp
			  è andato tutto su stdout(cioè nella pipe siccome ho usato dup)..
			   e ora devo leggere tutto dal figlio
			*/
			s_occ tmp;
			k=0;
			ritorno=0;
			tmp.c1=pid;

			while(read(new[0],&(tmp.c3[j]),1))
			{
				//Se siamo arrivati alla fine di una linea
				if(tmp.c3[j] == '\n')
				{
					ritorno++;
					j++;
					tmp.c3[j]='\0';
					tmp.c2=ritorno;

					//Il figlio comunica al padre
					write(piped[i][1], &tmp, sizeof(tmp));
					j=0;
				}
				else
				{
					j++;
				}
			}

			//printf("HO MANDATO QUESTO %s ***\n",tmp.c3);


			//Il figlio aspetta il nipote
			wait(NULL);
			exit(0);

		}

	}
	//caso del padre
	if(pid > 0)
	{
		s_occ tmp;

		printf("Sono il padre di PID %d e sono partito\n",getpid());

		//Chiudo le pipe che non uso..
		//Siccome il padre ha solo il compito di LEGGERE dai figli
		//In SCRITTURA chiudo tutto
		//In LETTURA lascio aperto
		for(i=0; i<N; i++)
		{
			close(piped[i][1]);
		}

		//Il padre recupera le informazioni dai figli in ordine..
		for(i=0; i<N; i++)
		{
			nr = read(piped[i][0],&tmp,sizeof(tmp));
			if(nr != sizeof(tmp))
			{
				printf("Lettura FALLITA da parte del padre dal %d-esimo figlio\n",i);
				exit(-1);
			}
			printf("Il pid del nipote e' '%d' il numero della linea corrispondete e' '%d' e la linea ricevuta e':\n",tmp.c1,tmp.c2);
			printf("***%s***\n",tmp.c3);
		}




		//Il padre aspetta i figli
		for(i=0; i<N; i++)
		{
			wait(NULL);
		}

		printf("Sono il padre di PID %d ed e' terminato tutto correttamente\n",getpid());

		exit(0);
	}














}
