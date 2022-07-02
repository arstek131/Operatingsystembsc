#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

/* definisco un tipo per un array di due interi */
typedef int pipe_t[2];

typedef struct 
{
	char chr;
	long int occ;
	char proc;
	int pid;
	
}carattere;


int main(int argc, char const *argv[])
{
	pipe_t *piped;
	int i,j;
	pipe_t np0,np1;
	int fd;
	char AM,ch;
	int nr,nw;
	long int Nocc;
	int first=0;
	int Pocc=0;
	carattere tmp;
	int pid;
	int prime=0;

	//Controllo il numero di argomenti
	if(argc < 5 || (argc-1)%2)
	{
		printf("Numero di argomenti non corretto\n");
		exit(1);
	}

	int M = argc-1;
	int N=M/2;

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

	//Comincio a generare i figli
	for(i=0; i<N; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Chiamata a funzione fork() fallita\n");
			exit(4);
		}	
		//Caso del figlio
		if(pid == 0)
		{
			printf("Sono il figlio di indice %d e pid %d e sono partito...\n",i,getpid());
			//Chiudo le pipe che non uso..
			//Siccome il figlioha solo il compito di scrivere al padre
			//In LETTURA chiudo tutto
			//In SCRITTURA lascio aperta la i-esima pipe
			for(j=0; j<N; j++)
			{
				close(piped[j][0]);
				if(i != j)
					close(piped[j][1]);
			}

			//Fra figlio e nipote devo creare 2 pipe di comunicazione
			//np0 e np1, quindi le creo
			if(pipe(np0)<0)
			{
				printf("Errore creazione della pipe np0\n");
				exit(-1);
			}
			if(pipe(np1)<0)
			{
				printf("Errore creazione della pipe np1\n");
				exit(-1);
			}

			//Passo alla creazione del nipote (per ogni figlio)
			pid=fork();
			if(pid < 0)
			{
				printf("Errore chiamata a funzione fork() nel nipote\n");
				exit(-1);
			}
			//Caso del nipote
			if(pid == 0)
			{
				printf("Sono il nipote di pid %d e sono partito\n",getpid());
				//Chiudo la pipe di comunicazione rimasta aperta fra figlio e padre
				close(piped[i][1]);

				//Il nipote chiude i lati della pipe che non gli servono
				//in comunicazione con il padre
				close(np0[1]);
				close(np1[0]);

				fd=open(argv[i+N+1],O_RDONLY);
				if(fd < 0)
				{
					printf("Errore apertura %s\n",argv[i+N+1]);
					exit(-1);
				}

				//Il nipote legge dal figlio il carattere da ricercare
				nr=read(np0[0],&AM,1);
				if(nr != sizeof(char))
				{
					printf("Errore lettura carattere AM da parte del nipote\n");
					exit(-1);
				}

				Nocc=0;
				while(read(fd,&ch,1)==1)
				{
					if(AM == ch)
						Nocc++;
				}

				//Mando questa info al figlio
				nw = write(np1[1],&Nocc,sizeof(Nocc));
				if(nw != sizeof(Nocc))
				{
					printf("Scrittura fallita sulla pipe da parte del nipote\n");
					exit(-1);
				}

				exit(0);
			}
			//Torno nel figlio
			//Chiudo le pipe nei confronti del nipote che non uso
			close(np0[0]);
			close(np1[1]);

			fd=open(argv[i+1], O_RDONLY);
			if(fd < 0)
			{
				printf("Errore apertura %s\n",argv[i+1]);
				exit(-1);
			}

			Pocc=0;
			while(read(fd,&ch,1)==1)
			{
				if(isupper(ch))
				{
					if(prime == 0)
					{
						AM=ch;
						Pocc++;
						write(np0[1],&AM,1);
						prime=0;
					}
					else if(ch == AM)
					{
						Pocc++;
					}

				}
			}

			read(np1[0],&Nocc,sizeof(Nocc));
			if(Pocc < Nocc)
			{
				tmp.occ=Nocc;
				tmp.proc='N';
				tmp.pid=pid;
			}
			else
			{
				tmp.occ=Pocc;
				tmp.proc='F';
				tmp.pid=getpid();
			}
			tmp.chr=AM;
			write(piped[i][1],&tmp,sizeof(tmp));

			//Il figlio aspetta il nipote
			wait(NULL);

			exit(0);


		}
	}
	//caso del padre
	printf("Sono il padre di pid %d e sono partito\n",getpid());

	//Chiudo le pipe nel padre che non uso
	//Siccome il padre non deve scrivere nulla
	//In scrittura chiudo tutto
	for(i=0; i<N; i++)
	{
		close(piped[i][1]);
	}

	for(i=0; i<N; i++)
	{
		read(piped[i][0],&tmp,sizeof(tmp));
		if(tmp.proc == 'N')
		{
			printf("Il npote di pid %d ha calcolato %ld occorrenze del carattere %c per %s\n",tmp.pid,tmp.occ,tmp.chr,argv[i+1]);
		}
		else if(tmp.proc == 'S')
		{
			printf("Il figlio di pid %d calcolato %ld occorrenze del carattere %c per %s\n",tmp.pid,tmp.occ,tmp.chr,argv[i+1]);
		}
	}

	//Il padre aspetta i figli
	for(i=0; i<N; i++)
	{
		wait(NULL);
	}

	exit(0);


	
}