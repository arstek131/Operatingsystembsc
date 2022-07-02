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
	int lunghezza;
	char ch;
	int i,j;
	int *pid;
	pipe_t *piped; //array di pipe per comunicazione padre figlio
	pipe_t p; //una sola pipe per comunciazione figlio nipote

	if(argc < 3)
	{
		printf("Numero parametri sbagliato\n");
		exit(1);
	}

	int nFigli=argc-1;

	//Creo l'array di pipe
	piped = (pipe_t *)malloc(nFigli*sizeof(pipe_t));

	if(piped == NULL)
	{
		printf("L'allocazione dell'array di pipe e' fallita\n");
		exit(2);
	}

	//Creo le pipe M pipe (ovvero nFiglie)
	for(i=0; i<nFigli; i++)
	{
		if(pipe(piped[i]) < 0)
		{
			printf("Errore nella creazione delle pipe\n");
			exit(3);	
		}
	}

	//Creo l'array di pid
	pid = (int *)malloc(nFigli * sizeof(int));

	if(pid == NULL)
	{
		printf("Errore allocazione array di pid\n");
		exit(3);
	}

	for(i=0; i<nFigli; i++)
	{
		pid[i]=fork();
		if(pid[i] < 0)
		{
			printf("Errore invocazione funzione fork()\n");
			exit(4);
		}
		//caso del figlio
		else if(pid[i] == 0)
		{
			printf("Sono il %d figlio con pid %d e sono partito\n",i,getpid());

			//Chiudo le pipe che non mi servono..
			//Siccome il figlio ha solo il compito di scrivere nei confronti del padre
			//In LETTURA chiudo tutto
			//In SCRITTURA chiudo tutto tranne il figlio preso in esame
			for(j=0; j<nFigli; j++)
			{
				close(piped[j][0]);
				if(i != j)
					close(piped[j][1]);
			}

			//Creo la pipe di comunicazione fra nipote e figlio
			if(pipe(p) < 0)
			{
				printf("Errore nella creazione della pipe tra figlio e nipote\n");
				exit(5);
			}

			//Creo effettivamente il nipote (per ogni figlio)
			if((pid[i]=fork()) < 0)
			{
				printf("Errore nella creazione del %d nipote\n",i);
				exit(6);
			}

			//Parte il nipote
			if(pid[i] == 0)
			{
				printf("Sono il processo nipote del figlio di indice %d e pid %d\n",i,getpid());

				//Chiudo la pipe rimasta aperta di comunicazione tra padre-figlo
				close(piped[i][1]);

				//Chiudo la stdin..
				close(0);
				//Apro il file di competenza, in questo modo il processo stesso
				//diventa stdin
				if(open(argv[i+1],O_RDONLY) < 0)
				{
					printf("Errore nella open del file %s\n",argv[i+1]);
					exit(7);
				}

				//Ora chiudo lo stdout, perchè la exec di default scrive su stdout
				//perchè io il messaggio non lo voglio su schermo ma devo buttarlo
				//sulla pipe per la comunicazione tra nipote e figlio
				close(1);
				dup(p[1]);

				//Ora ogni nipote può chiudere entrambi i lati della pipe
				//Il lato 0 non viene usato
				//il lato 1 viene usato da stdout
				close(p[0]);
				close(p[1]);

				//Ridirezione anche lo stderror su /dev/null (per evitare messaggi di errore a video)
				close(2);
				open(" /dev/null", O_WRONLY);

				//Quindi ora praticmante il nipote diventa il comando tail -1
				execlp("tail", "tail", "-1", (char *)0);

				exit(-5);

			}
			//Sono fuori dal nipote e torno di nuovo nel figlio
			else
			{
				//Ogni figlio deve chiudere il lato che non usa della pipe
				//	di comunicazione con il nipote
				close(p[1]); //chiudo la scrittura nei confronti del nipote

				//Il figlio legge dalla pipe di comunicazione tra lui e nipote
				//	e legge la linea di cui ora il figlio andrà a calcolare la lunghezza
				//	ecluso il carattere terminatore

				j=0;
				while(read((p[0]),&ch,1))
				{
					j++;

				}

				//Se la read ha letto qualcosa
				if(j != 0)
				{
					//Faccio -1 per il carattere terminatore
					lunghezza=j-1;
				}
				else
				{
					lunghezza=0;
				}
				printf("\n******HO LETTO %d CARATTERI********\n",j);

				//Ora il figlio comunica con il padre
				write(piped[i][1],&lunghezza,sizeof(lunghezza));

				//Giustamente il figlio deve aspettare il nipote prima
				//	di poter passare qualcosa al padre..
				wait(NULL);

				exit(0);


			}




		}

	}
	//caso del padre
	if(pid[i] > 0)
	{
		printf("Sono il padre di pid %d e sono partito\n",getpid());

		//Chiudo le pipe che non mi servono..
		//Siccome il padre ha solo il compito di leggere
		//In SCRITTURA chiudo tutto
		//In LETTURA lascio aperto
		for(i=0; i<nFigli; i++)
		{
			close(piped[i][1]);
		}

		//Il padre recupera le informazioni in ordine inverso (richiesto dalla traccia)
		for(i=nFigli-1; i>=0; i--)
		{
			read(piped[i][0], &lunghezza, sizeof(lunghezza));
			printf("*******Il figlio di indice %d ha comunicato il valore %d per il file %s\n",i,lunghezza,argv[i+1]);
		}

		//Il padre giustamente aspetta i figli
		for(i=0; i<nFigli; i++)
		{
			wait(NULL);
		}

		exit(0);


	}























}