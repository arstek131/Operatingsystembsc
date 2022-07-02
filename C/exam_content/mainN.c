#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>
#include <ctype.h>

typedef int pipe_t[2];

int main(int argc, char const *argv[])
{
	
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

	//Creo le pipe per comunicazione tra Padre-Figlio
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
				execlp("tail", "tail", "-1", (char *)0);


			}
			else
			{
				//Sono fuori dal nipote e torno nel figlio

				//Siccome il figlio non deve scrivere nulla..
				//Chiudo la pipe in SCRITTURA tra Figlio-Nipote
				close(new[1]);

				//Ora mi devo calcolare la lunghezza della linea
				//Che mi ha inviato il nipote, in pratica sto leggendo dalla pipe
				k=0;
				while(read(new[0],&ch,1)==1)
				{
					if(ch == '\n')
						break;
					k++;
				}

				//printf("****STAMPA DEBUG %d %s *****\n",k,argv[i+1]);
				//Ora il figlio comunica al padre la lunghezza..
				nw = write(piped[i][1],&k,sizeof(int));
				if(nw != sizeof(int))
				{
					printf("Scrittura da parte del figlio di pid %d fallita sulla pipe\n",getpid());
					exit(-1);
				}

				/*//Il Figlio aspetta il nipote
				wait(NULL);
				exit(0);*/

				ritorno=-1;
				pid = wait(&status);
				if (pid < 0)
				{	
					printf("Errore in wait\n");
					exit(-6);
				}
				if ((status & 0xFF) != 0)
				{
	    			printf("Nipote con pid %d terminato in modo anomalo\n", pid);
				}
	    		else
	    		{
					printf("Il nipote con pid=%d ha ritornato %d\n", pid, ritorno=(int)((status >> 8) & 0xFF));
	    		}
				exit(ritorno);
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
	for(i=N-1; i>=0; i--)
	{
		nr = read(piped[i][0],&k,sizeof(k));
		if(nr != sizeof(k))
		{
			printf("Lettura fallita dalla pipe da parte del padre\n");
			exit(-1);
		}
		printf("Il figlio di indice %d e pid %d ha calcolato la lunghezza di %d per il file %s\n",i,pid[i],k,argv[i+1]);

	}

	/* Il padre aspetta i figli */
	for (i=0; i < M; i++)
	{
		pid = wait(&status);
		if (pid < 0)
		{
			printf("Errore in wait\n");
			exit(5);
		}

		if ((status & 0xFF) != 0)
		{
			printf("Figlio con pid %d terminato in modo anomalo\n", pid);
		}
    	else
		{ 
		  ritorno=(int)((status >> 8) &0xFF); 

		  if (ritorno==255)
		  {
		  	printf("Il figlio con pid=%d ha ritornato %d e quindi vuole dire che il nipote non e' riuscito ad eseguire il tail oppure e' terminato in modo anormale\n", pid, ritorno);
		  }
 			
		  else  
		  {
		  	printf("Il figlio con pid=%d ha ritornato %d\n", pid, ritorno);
		  }
		  	
		}
	}

exit(0);

}