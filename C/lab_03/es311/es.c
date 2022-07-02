#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

typedef int pipe_t[2];


int main(int argc, char const *argv[])
{
	int nr,nw;
	int pid;
	int i,j,k;
	pipe_t *piped;
	pipe_t *mpiped;
	char ch;

	//Controllo argomenti
	if(argc < 3)
	{
		printf("Numero argomenti sbagliato\n");
		exit(1);
	}

	int nFigli=atoi(argv[1]);
	int mNipoti=atoi(argv[2]);



	//Creo l'array di pipe per la comunicazione tra padre - figlio
	piped = (pipe_t *)malloc(nFigli*sizeof(pipe_t));

	if(piped == NULL)
	{
		printf("Allocazione dell'array di pipe fallita\n");
		exit(2);
	}

	//Creo le pipe
	for(i=0; i<nFigli; i++)
	{
		if(pipe(piped[i]) < 0)
		{
			printf("Errore nella chiamata a funzione pipe()\n");
			exit(3);
		}
	}

	//Genero i figli
	for(i=0; i<nFigli; i++)
	{
		pid=fork();
		if (pid < 0)
		{
			printf("Errore chiamata a funzione fork()\n");
			exit(4);
		}

		//Caso del figlio
		if(pid == 0)
		{
			printf("Sono il %d figlio con pid %d e ho cominciato\n",i,getpid());

			//Creo l'array con M pipe per la comunicazione figlio-nipote
			mpiped = (pipe_t *)malloc(mNipoti*sizeof(pipe_t));

			if(mpiped == NULL)
			{
				printf("Errore nell'allocazione dell'array di pipe per i nipoti\n");
				exit(6);
			}

			//Chiudo le pipe che non uso
			//Siccome il figlio ha solo il compito di leggere dal padre..
			//	In LETTURA quindi lascio aperto solo i-esimo figlio
			//	In SCRITTURA chiudo tutto..
			for(j=0; j<nFigli; j++)
			{
				close(piped[j][1]);
				if(i != j)
					close(piped[j][0]);
			}

			nr=read(piped[i][0],&ch,1);

			if(nr != 1)
			{
				printf("Errore lettura del carattere da padre a figlio\n");
				exit(8);
			}

			//printf("Sono il %d figlio e ho letto correttamente il carattere %c\n",i,ch );

			

			//Genero le M pipe per la comunicazione figlio-nipote
			for(j=0; j<mNipoti; j++)
			{
				if(pipe(mpiped[j]) < 0)
				{
					printf("Errore nella chiamata a funzione pipe() per l'array di nipoti\n");
					exit(7);
				}
			}

			
			//Creo gli M nipoti
			for(j=0; j<mNipoti; j++)
			{
				pid=fork();
				if(pid < 0)
				{
					printf("Errore chiamata fork() per creazione nipote\n");
					exit(8);
				}
				//Sono nel nipote
				if(pid == 0)
				{
					printf("Sono il nipote di indice %d e pid %d\n",j,getpid());

					//chiudo la pipe rimasta aperta tra figlio e padre
					//close(piped[i][0]);

					for(k=0; k<mNipoti; k++)
					{
						close(mpiped[k][1]);
						if(k != j)
							close(mpiped[k][0]);
					}

					nr=read(mpiped[j][0],&ch,1);
					if(nr != 1)
					{
						printf("Errore lettura del carattere da padre a figlio\n");
						exit(9);
					}

					printf("Il nipote di pid %d ha letto il carattere %c\n",getpid(),ch);

					exit(0);

				}
			}

			for(k=0; k<mNipoti; k++)
			{
				close(mpiped[k][0]);
			}

			for(k=0; k<mNipoti; k++)
			{
				//Scrivo il carattere al nipote
				 write(mpiped[k][1],&ch,1);
			}

			//Ritorno al figlio
			for(k=0; k<mNipoti; k++)
			{
		    	wait(NULL);
			}

			exit(0);
					
		}
	}

	//Caso del padre
	if(pid > 0)
	{
		char tmp= 'a';

		printf("Sono il padre di pid %d e ho cominciato..\n",getpid());
		//Chiudo le pipe che non uso
		//Siccome il padre ha il comptio di scrivere ai figli
		//	In LETTURA chiudo tutto..
		//	In SCRITTURA lascio aperto tutto siccome il padre deve scrivere ai figli..
		for(i=0; i<nFigli; i++)
		{
			close(piped[i][0]);
		}

		//Il padre invia un carattere a tutti i figli
		for(i=0; i<nFigli; i++)
		{
			printf("Sto per scrivere sulla pipe al %d figlio\n",i);
			nw=write(piped[i][1],&tmp,1);
			if(nw != 1)
			{
				printf("Errore nella scrittura su pipe al %d figlio\n", i);
				exit(5);
			}
			tmp=tmp+1;
		}

		//Il padre aspetta la terminazione dei figli
		for(i=0; i<nFigli; i++)
		{
			wait(NULL);
		}

		printf("Sono il padre con pid %d ed e' terminato tutto correttamente\n", getpid());

		exit(0);

	}
	









}