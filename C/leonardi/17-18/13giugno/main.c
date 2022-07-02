













#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

/* definisco un tipo per un array di due interi */
typedef int pipe_t[2];

typedef struct 
{
	int pid;
	int lunghezza;
	
}s;

typedef struct 
{
	int pid; 
	int indice;
	int lunghezza;
}s_padre;

void bubbleSort(s_padre v[],int dim) /* il tipo degli elementi NON e' un semplice int come riportato sul testo, ma deve essere il tipo s_padre (appositamente definito) */
{
int i;
int ordinato=0;
s_padre a;  /* variabile di appoggio per fare lo scambio */
        while (dim>1 && ordinato!=1)
        {
                ordinato=1;
                for (i=0;i<dim-1;i++)
                if (v[i].lunghezza > v[i+1].lunghezza) /* chiaramente il confronto va fatto sul campo lunghezza della struttura */
                {  /* viene effettuato in questo caso un ordinamento in senso crescente */
                        /* scambio gli elementi */
                        a=v[i];
                        v[i]=v[i+1];
                        v[i+1]=a;
                        ordinato=0;
                }
                dim--;
        }
}/* fine bubblesort */



int main(int argc, char const *argv[])
{
	int nr;
	char ch;
	int fd;
	s *cur;
	s_padre *cur_padre;
	int pid;
	int i,j;
	int Y;
	int N;
	pipe_t *pipes;

	//Controllo il numero di parametri
	if(argc < 4)
	{
		printf("Numero di parametri sbagliato\n");
		exit(1);
	}

	//Mi salvo la lunghezza in linee del file
	Y=atoi(argv[argc-1]);
	if(Y < 0)
	{
		printf("Y non puo' essere < 0\n");
		exit(2);
	}

	N=argc-2;
	printf("Il numero dei processi da creare sara' %d\n",N);

	//Alloco le N pipe
	pipes = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(pipes == NULL)
	{
		printf("Errore nell'allocazione dell'array di pipe\n");
		exit(3);
	} 

	//Creo effettivamente le pipe
	for(i=0; i<N; i++)
	{
		if(pipe(pipes[i])<0)
		{
			printf("Errore nella chiamata a funzione pipe()\n");
			exit(4);
		}
	}

	//Creo gli N figli
	for(i=0; i<N; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Errore nella chiamata a funzione fork()\n");
			exit(5);
		}
		//caso del figlio
		else if(pid == 0)
		{
			printf("Sono il figlio %d e sono associato al file %s\n",getpid(),argv[i+1]);

			//Chiudo le pipe inutilizzate
			//In scrittura chiudo tutto tranne quella attuale

			for(j=0; j<N; j++)
			{
				if(j!=i)
					close(pipes[j][1]);
				if((i==0) || (j != i-1))
					close(pipes[j][0]);
			}

			//Creo l'array di struct
			cur = (s *)malloc((i+1)*sizeof(s));
			if(cur == NULL)
			{
				printf("Allocazione dell'array di struct fallita\n");
				exit(N);
			}

			//Inizializzo la struct
			cur[i].pid=getpid();
			cur[i].lunghezza=0;

			//Apro il file
			fd=open(argv[i+1],O_RDONLY);
			if(fd < 0)
			{
				printf("Impossibile aprire il file %s\n",argv[i+1]);
				exit(N+1);
			}

			while(read(fd,&ch,1)>0)
			{
				//se leggo un carattere, incremento la lunghezza della linea
				cur[i].lunghezza++;
				if(ch == '\n')
				{
					//Se siamo a fine linea, si deve leggere dal figlio precedente l'array
					//  (a meno che non siamo il primo figlio) e mandare la struct al next figlio
					if(i != 0)
					{
						nr=read(pipes[i-1][0],cur,i+sizeof(s));
						if(nr != sizeof(s)*i)
						{
							printf("Il figlio %d ha letto un numero di strutture sbagliate %d\n",i,nr);
							exit(N+2);
						}
						for(j=0; j<N; j++)
						{
							printf("Ho ricevuto dal figlio di pid %d la lunghezza %d\n",cur[j].pid,cur[j].lunghezza);

						}

					}

					//Tutti i figli mandano in avanti, tranne l'ultimo figlio che manda al padre
					// un array di strutture
					write(pipes[i][1],cur,(i+1)*sizeof(s));
					cur[i].lunghezza=0;
				}
			}
			exit(i);


		}
	}
	//caso del padre
	if(pid > 0)
	{
		printf("Sono il padre di pid %d\n",getpid());

		//Chiudo le pipe inutilizzate, ovvero..
		// In scrittura chiudo tutto (il padre non deve scrivere nulla)
		// lettura tutto tranne l'ultima, siccome è da dove il padre deve leggere
		for(i=0; i<N; i++)
		{
			close(pipes[i][1]);
			if(i != N-1)
				close(pipes[i][0]);
		}

		//Creo l'array di strutture per il padre..
		cur = (s *)malloc(N * sizeof(s));
		if(cur == NULL)
		{
			printf("Errore nell'allocazione dell'array di struct dal padre\n");
			exit(7);
		}

		//Alloco l'array di struct specifico per il padre per fare l'ordinamento
		cur_padre = (s_padre *)malloc(N*sizeof(s_padre));
		if(cur_padre == NULL)
		{
			printf("Errore nell'allocazione dell'array cur_padre\n");
			exit(8);
		}



		//Il padre deve leggere via via gli array di struct che gli arrivano dall'ultimo
		// figlio, uno per linea
		for(j=1; j<=Y; j++)
		{
			nr=read(pipes[N-1][0],cur,N*sizeof(s));
			if(nr != N*sizeof(s))
			{
				printf("Il padre ha letto un numero di struct sbagliate %d\n",nr);
				exit(9);
			}

			//Il padre copia l'array ricevuto nel proprio array..
			for(i=0; i<N; i++)
			{
				cur_padre[i].pid=cur[i].pid;
				cur_padre[i].indice=i;
				cur_padre[i].lunghezza=cur[i].lunghezza;
			}

			//Ordino l'array cur_padre
			bubbleSort(cur_padre,N);

			//Stampo le info
			for(i=0; i<N; i++)
			{
				printf("Il figlio con pid %d per il file %s ha calcolato per la linea %d la lunghezza %d\n", cur_padre[i].pid, argv[cur_padre[i].indice + 1], j, cur_padre[i].lunghezza);
			}

		}

		//Il padre aspetta i figli
		for(i=0; i<N; i++)
		{
			wait(NULL);
		}


		exit(0);
	}





}
