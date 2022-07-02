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
	int indice;
	int lunghezza;
	
}s_padre;

typedef struct 
{
	int c1;
	int c2;
	
}s_occ;

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
	s_occ tmp;
	pipe_t *piped;
	int *pid;
	int i,j;
	s_occ *cur;
	int fd;
	char ch;
	int nr,nw;
	s_padre *cor;


	//Controllo il numero di parametri
	if(argc < 4)
	{
		printf("Numero di parametri sbagliato\n");
		exit(1);
	}

	int Y = atoi(argv[argc-1]);
	if(Y <= 0)
	{
		printf("Ultimo parametro deve essere >0\n");
		exit(2);
	}

	int N = argc-2;

	//Creo l'array di pipe per la comunicazione tra i Figli e infine con il padre
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

	//Creo l'array di pid
	pid = (int *)malloc(N*sizeof(int));
	if(pid == NULL)
	{
		printf("Allocazione array di pid fallita\n");
		exit(5);
	}

	//Creo gli N figli
	for(i=0; i<N; i++)
	{
		pid[i]=fork();
		if(pid[i] < 0)
		{
			printf("Chiamata a funzione fork() fallita\n");
			exit(5);
		}
		//Caso del figlio
		if(pid[i] == 0)
		{
			printf("Sono il figlio di pid %d e indice %d sono partito..\n",getpid(),i);

			//Chiudo le pipe che non uso..
			//Siccome il primo figlio deve attivare la pipeline
			//In SCRITTURA lascio aperta la i-esima pipe mentre le altre le chiudo
			//In LETTURA se siamo al primo giro, chiudo tutto
			//	se siamo al 2 giro o più lascio aperta quella dietro e chiudo le altre

			for(j=0; j<N; j++)
			{
				if(i != j)
					close(piped[j][1]);
				if(i==0)
					close(piped[j][0]);
				else
				{
					if(i!=j+1)
						close(piped[j][0]);
				}
			}

			//Creo l'array di strutture per mandarlo man mano in avanti ai figli
			//Creo un array di dimensione i+1 anche se leggo i strutture
			//Dato che tutte le volte riempiamo la i+1-esima struttura
			//Mentre le altre le prendiamo con la read..
			cur=(s_occ *)malloc((i+1)*sizeof(s_occ));
			if(cur == NULL)
			{
				printf("Allocazione arry di cur fallito dal %d figlio\n",i);
				exit(-1);
			}

			//Inizializzo la struttura..
			cur[i].c1=getpid();
			cur[i].c2=0;

			fd=open(argv[i+1],O_RDONLY);
			if(fd < 0)
			{
				printf("Impossibile aprire il file %s\n",argv[i+1]);
				exit(-1);
			}

			while(read(fd,&ch,1)==1)
			{
				//Se leggo un carattere incremento la lunghezza
				cur[i].c2++;
				if(ch == '\n')
				{
					//Se siamo a fine linea, si deve leggere dal figlio precedente
					//l'array (se non siamo il primo figlio) e mandare la struttura
					//al figlio successivo
					if(i!=0)
					{
						//Ne leggo i.. perchè io la mia roba la ho nella i+1
						//di conseguenza tutte le vecchie le riempio con la read
						//Che si vanno a concatenare con l'ultima riempita manualmente
						nr=read(piped[i-1][0],cur,i*sizeof(s_occ));
						if(nr != sizeof(s_occ))
						{
							printf("Lettura dal figlio %d sbagliata\n",i);
							exit(-1);
						}

					}
					write(piped[i][1],cur,(i+1)*sizeof(s_occ));
					cur[i].c2=0;
				}
			}


			exit(i);

		}
	}
	//Caso del padre
	printf("Sono il padre di pid %d e sono partito..\n",getpid());

	//Chiudo le pipe che non uso..
	//Siccome il padre ha solo il compito di leggere
	//In SCRITTURA chiudo tutto
	//In LETTURA lascio aperta l'ultima pipe

	for(i=0; i<N; i++)
	{
		close(piped[i][1]);
		if(i!=N-1)
			close(piped[i][0]);
	}

	//Allocazione dell'array di struct specifico per il padre, per la read
	//Creaimo un array di dimensione N quanto il numero di figli
	cur = (s_occ *)malloc(N*sizeof(s_occ));
	if(cur == NULL)
	{
		printf("Errore allocazione array di s_occ nel padre\n");
		exit(-1);
	}

	//Alloco l'array di struct specifico per il padre per l'ordinamento
	//Creo un array di dimensione N quanto il numero dei figli
	cor=(s_padre *)malloc(N*sizeof(s_padre));
	if(cor == NULL)
	{
		printf("Errore allocazione array di s_padre nel padre\n");
		exit(-1);
	}




	//Il padre deve leggere via via gli array che gli arrivano dall'ultimo figlio
	//Per Y volte (cioè per ognuna delle linee)
	for(j=0; j<Y; j++)
	{
		nr=read(piped[N-1][0],cur,N*sizeof(s_occ));
		if(nr != N*sizeof(s_occ))
		{
			printf("Il padre ha letto un numero di strutture sbagliate %d\n",nr);
			exit(-1);
		}
		for(i=0; i<N; i++)
		{
			//Il padre copia l'array ricevuto nel proprio array
			cor[i].pid=cur[i].c1;
			cor[i].indice=i;
			cor[i].lunghezza=cur[i].c2;
		}
		//Ordino l'array cur_padre
		bubbleSort(cor,N);

		//Stampo le informazioni
		for(i=0; i<N; i++)
		{
			printf("Il figlio con pid %d e per il file %s ha calcolato per la linea %d la lunghezza %d\n",cor[i].pid,argv[cor[i].indice+1],j+1,cor[i].lunghezza);

		}

		
	}


	//Il padre aspetta i figli
	for(i=0; i<N; i++)
	{
		wait(NULL);
	}

	exit(0);




	




















}