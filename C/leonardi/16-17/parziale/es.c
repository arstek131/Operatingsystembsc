#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

typedef int pipe_t[2];
typedef struct
{
	long int occ;
	int id;
	long int somma;
	
}s_occ;


int main(int argc, char **argv)
{
	int pidFiglio,status,ritorno; //valore di ritorno filgi
	int fd;
	long int cur_occ=0;
	int i,j;
	int N; //Numero totale di file da creare
	int *pid; //array di pid per fare poi la fork()
	char Cx,ch; //carattere da cercare
	pipe_t *pipes; //array di pipe
	s_occ letta; //struttura dati usata dal figlio e dal padre
	int nr, nw; //variabili per salvare i valori di ritorno di read/write

	//Controllo il numeri di parametri, almeno 2 file e un carattere
	if(argc < 4)
	{
		printf("Errore nel numero di parametri\n");
		exit(1);
	}
	//controllo che l'ultimo parametro sia un singolo carattere
	if(strlen(argv[argc-1])!=1)
	{
		printf("Errore l'ultimo parametro non e' un singolo carattere\n");
		exit(2);
	}

	//Mi salvo in Cx il carattere da cercare
	Cx=argv[argc-1][0];
	printf("Il carattere da cercare e' %c\n",Cx);

	N=argc-2; //perchè esluco ./nomefile e l'ultimo carattere
	printf("Il numero di processi da creare e' %d\n",N);

	//alloco l'array di pid
	if((pid=(int *)malloc(N*sizeof(int)))==NULL) //se l'allocazione dovesse fallire
	{
		printf("Errore allocazione pid\n");
		exit(3);
	}

	//allocazione pipe
	if((pipes=(pipe_t *)malloc(N*sizeof(pipe_t)))==NULL)
	{
		printf("Errore allocazione pipe\n");
		exit(4);
	}

	//creo le pipe
	for(i=0; i<N; i++)
	{
		if(pipe(pipes[i])<0)
		{
			printf("Errore nella creazione pipe\n");
			exit(5);
		}

	}

	//Creo i figli
	for(i=0; i<N; i++)
	{
		if((pid[i]=fork())<0)
		{
			printf("Errore nella creazione del figlio\n");
			exit(6);
		}
		//parte il codice del figlio
		else if(pid[i]==0)
		{
			printf("Sono il figlio %d\n",getpid());
			//Chiudo pipe inutilizzate
			for(j=0; j<N; j++)
			{
				if(j!=i)
					close(pipes[j][1]);
				if((i==0) || (j!= i-1))
					close(pipes[j][0]);
			}

			//inizializzo il contatore di occorrenze
			cur_occ=0;

			//apro il file corrispondente ad ogni figlio
			if((fd=open(argv[i+1],O_RDONLY))<0)
			{
				printf("Impossibile aprire il file %s\n",argv[i+1]);
				exit(N);
			}
			//comincio la lettura di tutti i carattteri
			while(read(fd,&ch,1)>0)
			{
				//cerco il carattere
				if(ch==Cx)
				{
					cur_occ++;
					printf("Sono il figlio di indice %d e pid %d e ho trovato %ld occorrenze del carattere %c\n",i,getpid(),cur_occ,Cx);
				}
			}
			if(i==0)
			{
				//Il primo figlio (quindi quello di indice 0 deve preparare la struttura da mandare al prossimo figlio)
				letta.id=0;
				letta.occ=cur_occ;
				letta.somma=cur_occ;
			}
			else
			{
				nr=read(pipes[i-1][0],&letta,sizeof(s_occ));
				if(nr != sizeof(s_occ))
				{
					printf("Il figlio %d ha letto un numero di byte sbagliati%d\n",i,nr);
					exit(N+1);
				}

				printf("Ho ricevuto dal figlio di indice %d che ha trovato %ld occorrenze del carattere %c\n",letta.id,letta.occ,Cx);

				if(letta.occ < cur_occ)
				{
					//Se il figlio di indice i ha calcolato un numero di occorrenze maggiore
					//bisogna aggiornare i valori di letta
					letta.id=i;
					letta.occ=cur_occ;
				}
				//Aggiorniamo comunque il valore della somma
				letta.somma=letta.somma+cur_occ;
			}
			//Tutti i figli mandano in avanti, l'ultmo figlio manda al padre una struct
			nw=write(pipes[i][1],&letta,sizeof(s_occ));
			if(nw!=sizeof(s_occ))
			{
				printf("Il figlio %d ha scritto un numero di byte sbagliati %d\n",i,nw);
				exit(N+2);
			}
			//ogni figlio deve tornare il proprio indice d'ordine
			exit(i);

		}
	}
	//Qua finisce il for
	//Inizia il codice del padre
	//chiudo tutte le pipe, tranne l'ultima in lettura
	for(i=0; i<N; i++)
	{
		close(pipes[i][1]); //le chiudo tutte in scrittura
		if(i!=N-1)
			close(pipes[i][0]);//chiudo tutte le pipe tranne l'ultima in lettura
	}
	//Il padre ha il compito di leggere la struttura che gli arriva dall'ultimo figlio
	nr=read(pipes[N-1][0],&letta,sizeof(s_occ));
	if(nr!=sizeof(s_occ))
	{
		printf("Il padre ha letto un numero di byte sbagliati %d\n",nr);
		exit(8);
	}

	//Il padre ora deve stampare i campi della struct ricevuta
	printf("Il figlio di indice %d e pid %d ha trovato il numero massimo di occorrenze %ld del carattere %c nel file %s\n",letta.id,pid[letta.id],letta.occ,Cx,argv[letta.id+1]);
	printf("I figli hanno trovato in totale %ld occorrenze del carattere %c nei file\n",letta.somma,Cx);

	//Il padre aspetta i figli
	for(i=0; i<N; i++)
	{
		pidFiglio=wait(&status);
		if(pidFiglio<0)
		{
			printf("Errore nella wait\n");
			exit(9);
		}

		if((status & 0XFF)!=0)
		{
			printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
		}
		else
		{
			ritorno=(int)((status >> 8) & 0xFF);
			printf("Il figlio con pid=%d ha ritornato %d (se > di %d problemi)\n",pidFiglio,ritorno,N-1);
		}

	}



exit(0);

}