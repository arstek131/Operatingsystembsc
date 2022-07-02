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
	int pid;
	pipe_t *piped;
	int i,j;
	int nr,nw;
	char comArr[255];
	char myArr[255];
	int k;
	int value;

	//Controllo il numero di parametri
	if(argc < 4)
	{
		printf("Numero di argomenti sbagliato\n");
		exit(1);
	}

	int H = atoi(argv[argc-1]);
	if(H <= 0)
	{
		printf("L'ultimo parametro deve essere un numero > 0\n");
		exit(2);
	}

	int N = argc-2;

	//Creazione dell'array per la comunicazione Padre-Figlio
	piped = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(piped == NULL)
	{
		printf("Allocazione array di pipe fallita\n");
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

	//Creo i figli
	for(i=0; i<N; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Errore chiamata a funzione fork()\n");
			exit(5);
		}
		//caso del figlio
		if(pid == 0)
		{
			printf("Sono il figlio di indice %d e pid %d e sono partito..\n",i,getpid());

			//Chiudo pipe che non uso
			for(j=0; j<N; j++)
			{
				close(piped[j][0]);
				if(i != j)
					close(piped[j][1]);
			}

			//Creo la pipe di comunicazione fra Figli-Nipote
			pipe_t new;
			if(pipe(new)<0)
			{
				printf("Creazione pipe Figli-Nipote fallita\n");
				exit(6);
			}

			//Creo il nipote
			pid=fork();
			if(pid < 0)
			{
				printf("Chiamta a funzione fork() fallita nel figlio\n");
				exit(-1);
			}
			//Caso del nipote
			if(pid == 0)
			{
				printf("Sono il nipote di pid %d e sono partito..\n",getpid());
				//Chiudo la pipe di comunicazione rimasta aperta tra Padre-Figlio
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

				//Chiudo tutto le pipe
				close(new[1]);
				close(new[0]);

				//Ridirezione anche lo stderror su /dev/null (per evitare messaggi di errore a video)
				close(2);
				open(" /dev/null", O_WRONLY);

				execlp("wc", "wc", "-l", (char *)0);

				exit(-1);

			}
			//Torno nel figlio

			//Chiudo la pipe in scrittura
			close(new[1]);

			k=0;
			while(read(new[0],&myArr[k],1)==1)
			{
				k++;
			}

			if(k!=0)
			{
				myArr[k-1]=0;
				value=atoi(myArr);
				if(value >= H)
				{
					sprintf(comArr,"Sopra media");
				}
				else
				{
					sprintf(comArr,"Sotto media");
				}
			}
			else
			{
				sprintf(comArr,"Sotto media");
			}

			//Comunico al padre la stringa
			write(piped[i][1],comArr,strlen(comArr)+1);


			//Il Figlio aspetta il nipote
			wait(NULL);

			exit(0);
		}
	}
	//caso del padre
	printf("Sono il padre di pid %d e sono partito\n",getpid());

	//Chiudo le pipe che non uso
	for(i=0; i<N; i++)
	{
		close(piped[i][1]);
	}

	
	for(i=0; i<N; i++)
	{
		read(piped[i][0],comArr,255);
		printf("Per il file %s si ha: %s\n",argv[i+1],comArr);

	}



	//Il padre aspetta i figli
	for(i=0; i<N; i++)
	{
		wait(NULL);
	}

	exit(0);


















	
}