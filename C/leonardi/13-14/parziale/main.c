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
	pipe_t *piped;
	int pid;
	int i,j;
	pipe_t p;
	char numero[11];
	int valore;
	long int somma=0;

	//Controllo numero di argomenti
	if(argc < 3)
	{
		printf("Numero di argomenti deve essere > 2\n");
		exit(1);
	}

	int nFigli = argc-1;

	//Creo l'array di pipe per la comunicazione Padre-Figlio
	piped = (pipe_t *) malloc(nFigli*sizeof(pipe_t));
	if(piped == NULL)
	{
		printf("Errore nell'allocazione dell'array di pipe\n");
		exit(2);
	}

	//Creo le pipe
	for(i=0; i<nFigli; i++)
	{
		if(pipe(piped[i]) < 0)
		{
			printf("Errore nella creazione della pipe\n");
			exit(3);
		}
	}


	//Generazione di N processi figli
	for(i=0; i<nFigli; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Errore nella chiamata a funzione fork()\n");
			exit(4);
		}
		//caso del figlio
		if(pid == 0)
		{
			printf("Sono il %d figlio di pid %d e sono partito...\n",i,getpid());

			//Chiudo le pipe che non uso
			//Siccome il figlio ha solo il compito di scrivere
			//In lettura chiudo tutto
			//Per quanto riguarda la scrittura, lascio aperta solo la i-esima pipe
			for(j=0; j<nFigli; j++)
			{
				close(piped[j][0]);
				if(i != j)
					close(piped[j][1]);
			}

			//Posso passare a creare le pipe di comunicazione tra figlio e nipote
			if(pipe(p) < 0)
			{
				printf("Errore nella creazione della pipe tra figlio e nipote\n");
				exit(5);
			}

			//Posso andare a creare il nipote... Un nipote per ogni figlio
			if((pid=fork()) < 0)
			{
				printf("Errore nella chiamata a funzione fork() per la creazione del nipote\n");
				exit(6);
			}

			//parte il nipote..
			if(pid == 0)
			{
				printf("Sono il processo nipote del figlio di indice %d e pid %d sto per calcolar eil numero di linee del file %s\n",i, getpid(), argv[i+1]);

				//Chiudo la pipe rimasta apergta di comunicazione fra figli-padre
				//	 che il nipote non usa

				close(piped[i][1]);

				//Ridirigo lo stdin poichè devo usare la funzione execpl
				close(0);
				if(open(argv[i+1], O_RDONLY) < 0)
				{
					printf("Errore nell'apertura del file %s\n",argv[i+1]);
					exit(6);
				}

				//Siccome col nipote devo simulare il piping dei comandi
				//	uso la funzione dup() e prima chiudo lo stdout
				close(1);
				dup(p[1]);

				//Ogni nipote adesso può chiudere entrambi i lati della pipe.
				//	Lato 0 non viene usato
				//	Lato 1 viene usato tramite lo stdout
				close(p[0]);
				close(p[1]);

				//Ridirigo lo stderrore su /dev/null (per evitare messaggi di errore a video)
				close(2);
				open(" /dev/null", O_WRONLY);

				//Siccome abbiamo chiuso la stdin, il nipote diventa
				//	il comando wc -l
				execlp("wc", "wc", "-l", (char *)0);

				//Torno un valore diverso da 0 per indicare insuccesso
				exit(-1);

			}
			//Riparto con il codcie del figlio
			else
			{
				//Ogni figlio deve chiudere il lato ch enon usa della pipe di comunicazione
				//	con il nipote
				close(p[1]);

				//Adesso il figlio legge dalla pipe con il figlio (figlio dal nipote)
				j=0;
				while(read(p[0],&(numero[j]),1))
				{
					j++;
				}

				//Converto l'arrayd i char in stringa sostituendo allo '\n' il terminatore
				if(j!=0)
				{
					numero[j-1]='\0';
					valore=atoi(numero);
				}
				else
				{
					valore=0;
				}

				//Il figlio comunica al padre
				write(piped[i][1],&valore,sizeof(valore));

				//Giustamente il figlio prima di poter comunicare con il padre
				//	deve aspettare il nipote termini...

				wait(NULL);

				exit(0);

			}

		}
	}
	//caso del padre
	if(pid > 0)
	{
		printf("Sono il padre di pid %d e sono partito\n",getpid());

		//Chiudo le pipe che non uso..
		//Siccome il padre ha solo il compito di leggere, chiudo in scrittura tutto
		for(i=0; i<nFigli; i++)
		{
			close(piped[i][1]);
		}

		//Il padre recupera le informazioni dai figli in ordine di indice(non invernso perchè nel testo non è scritto)
		for(i=0; i<nFigli; i++)
		{
			read(piped[i][0],&valore,sizeof(valore));
			somma=somma+(long int)valore;
		}

		printf("La somma risultante derivante dai valori cominicati dai figli e' %ld\n",somma);

		//Il padre aspetta i figli
		for(i=0; i<nFigli; i++)
		{
			wait(NULL);
		}

		exit(0);

	}






















}