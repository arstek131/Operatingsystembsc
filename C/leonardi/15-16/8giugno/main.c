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

int mia_random(int n)
{
int casuale;
casuale = rand() % n;
return casuale;
}


int main(int argc, char const *argv[])
{
	int *pid;
	pipe_t *pf,*fp;
	int i,j;
	char ch;
	int lung=0;
	int nw,nr;
	int r;
	int value;
	int send;
	int indice=0;
	char myArr[255];
	int k=0;
	int fdMain;
	int fd;
	int ritorno=0;
	int status;


	//Controllo numero di parametri
	if(argc < 6)
	{
		printf("Numero di parametri sbagliato\n");
		exit(1);
	}

	int H = atoi(argv[argc-1]);
	if(H <=0 || H >= 255)
	{
		printf("H deve essere compreso tra 0 e 255\n");
		exit(2);
	}

	int N = argc-2;

	fdMain = open("/tmp/creato",O_CREAT|O_WRONLY|O_TRUNC,0644);
	if(fdMain < 0)
	{
		printf("Creazione del file Creato fallita\n");
		exit(3);
	}

	//Creo l'array di per per la comunicazione fra padre-figlio e figlio-padre
	if(pf = (pipe_t *)malloc(N*sizeof(pipe_t))==NULL)
	{
		printf("Allocazione array padre-figlio fallita\n");
		exit(4);
	}

	if(fp = (pipe_t *)malloc(N*sizeof(pipe_t))==NULL)
	{
		printf("Allocazione array figlio-padre fallita\n");
		exit(5);
	}

	//Creo le pipe
	for(i=0; i<N; i++)
	{
		if(pipe(pf)<0 || pipe(fp)<0)
		{
			printf("Chiamata a funzione pipe()fallita\n");
			exit(6);
		}
	}

	//Creo l'array di pid
	pid = (int *)malloc(N*sizeof(int));
	if(pid == NULL)
	{
		printf("Allocazione array di pid fallita\n");
		exit(7);
	}

	//Genero i figli
	for(i=0; i<N; i++)
	{
		pid[i]=fork();
		if(pid[i]<0)
		{
			printf("Chiamata a funzione fork() fallita\n");
			exit(8);
		}
		//caso del figlio
		if(pid[i] == 0)
		{
			printf("Sono il figlio di indice %d e pid %d e sono partito\n",i,getpid());
			//Chiudo le pipe che non uso
			for(j=0; j<N; j++)
			{
				close(fp[j][0]);
				close(pf[j][1]);
				if(i != j)
				{
					close(fp[j][1]);
					close(pf[j][0]);
				}
			}

			fd=open(argv[i+1],O_RDONLY);
			if(fd < 0)
			{
				printf("Errore apertura file %s dal %d figlio\n",argv[i+1],i);
				exit(9);
			}

			lung=0;
			indice=0;
			k=0;
			while(read(fd,&myArr[k],1)==1)
			{
				lung++;
				k++;
				if(ch == '\n')
				{
					nw = write(fp[i][1],&lung,sizeof(lung));
					if(nw != sizeof(lung))
					{
						printf("Errore scrittura su pipe da parte del %d figlio per il file %s\n",i,argv[i+1]);
						exit(-1);
					}
					nr = read(pf[i][0],&indice,sizeof(indice));
					if(nr != sizeof(indice))
					{
						printf("Lettura sbagliata da parte del %d figlio\n",i);
						exit(-1);
					}

					//Indice ammissibile
					if(indice < k)
					{
						write(fdMain,&myArr[indice],1);
						ritorno++;
					}

					lung=0;
					k=0;
				}
			}


			exit(ritorno);


		}
	}
	//Caso del padre
	printf("Sono il padre di pid %d e sono partito\n",getpid());
	srand(time(NULL));

	//Il padre chiude i lati delle pipe che non usa
	for(i=0; i<N; i++)
	{
		close(pf[i][0]);
		close(fp[i][1]);
	}

	//Il padre ora recupera le informazioni
	//Si saranno H cicli esterni (siccome per ogni linea viene mandato qualcosa nella pipe)
	//E al suo interno un ulteriore ciclo che cicla i figli

	for(j=1; j<=H; j++)
	{
		//Calcoliamo in modo random l'indice del figlio di cui si considererà il valore inviato
		r=mia_random(N);

		for(i=0; i<N; i++)
		{
			//Il padre recupera le info dai figli
			read(fp[i][0],&value,sizeof(value));
			//Se si tratta l'indice randomizzato che voglio..
			if(i==r)
				send=value;
		}
		//Calcolo in modo random l'indice della linea che invierà a tutti i figli
		r=mia_random(send);

		//Il padre invia a tutti i figli l'indice
		for(i=0; i<N; i++)
		{
			write(pf[i][1],&r,sizeof(r));
		}

	}


	//Il padre aspetta i figli
	for(i=0; i<N; i++)
	{
		pid[i]=wait(&status);
		if(pid < 0)
		{
			printf("Errore nella wait\n");
			exit(8);
		}
		if ((status & 0xFF) != 0)
		{
    		printf("Figlio con pid %d terminato in modo anomalo\n", pid);
		}
		else
		{
			ritorno=(int)((status >> 8) &	0xFF); 
			printf("Il figlio con pid=%d ha ritornato %d (se 255 ci sono stati errori nel figlio)\n", pid[i], ritorno);
		}

	}



	exit(0);

	

}