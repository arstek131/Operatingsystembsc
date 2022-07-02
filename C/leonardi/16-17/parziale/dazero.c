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
	int pidd;
}mod_2;

typedef struct 
{
	long int c1;
	int c2;
	long int c3;
}s_occ;

int main(int argc, char const *argv[])
{
	pipe_t *piped;
	int i,j;
	int pid;
	s_occ tmp;
	mod_2 tmp2;
	int nr,nw;
	int fd;
	char ch;

	//Controllo numero parametri
	if(argc < 4)
	{
		printf("Numero parametri < 4\n");
		exit(1);
	}

	int l = strlen(argv[argc-1]);
	if(l > 1)
	{
		printf("L'ultimo parametro non e' un singolo carattere\n");
		exit(2);
	}

	char Cx = argv[argc-1][0];

	int N = argc-2;

	//Creo l'array di pipe per la comunicazione tra Padre - Figlio
	piped = (pipe_t *)malloc(N*sizeof(pipe_t));

	//Creo le pipe
	for(i=0; i<N; i++)
	{
		if(pipe(piped[i]) < 0)
		{
			printf("Chiamata a funzione pipe() fallita\n");
			exit(3);
		}
	}

	//Creo gli N figli
	for(i=0; i<N; i++)
	{
		pid = fork();
		if(pid < 0)
		{
			printf("Chiamata a funzione fork() fallita\n");
			exit(4);
		}
		//caso del figlio
		if(pid == 0)
		{
			printf("Sono il figlio di indice %d e pid %d e sono partito..\n",i,getpid());

			//Chiudo le pipe che non uso, ovvero..
			//Siccome ogni figlio deve comunicare con il prossimo
			//	In SCRITTURA chiudo tutto tranne quello attuale
			//	In LETTURA se siamo al 1 giro, chiudo tutto (siccome la prima volta nessun figlio deve leggere)
			//		se siamo al 2 giro o più lascio aperta la pipe dietro e chiudo le altre

			for(j=0; j<N; j++)
			{
				if(j != i)
				close(piped[j][1]);
				if(i==0)
				{	
					close(piped[j][0]);
				}
				else
				{
					if(i!=j+1)
						close(piped[j][0]);
				}

			}

			//Apro il file di competenza
			fd = open(argv[i+1],O_RDONLY);
			if(fd == -1)
			{
				printf("Errore apertura del %s file\n",argv[i+1]);
			}

			int occor=0;

			while(read(fd,&ch,1) > 0)
			{
				if(ch == Cx)
				{
					occor++;
				}
			}

			//Se si tratta del primo figlio.. preparo la prima volta la struttura
			if(i==0)
			{
				tmp.c1=occor;
				tmp.c2=i;
				tmp.c3=0;
				tmp2.pidd=getpid();

				nw = write(piped[0][1],&tmp,sizeof(tmp));
				if(nw != sizeof(tmp))
				{
					printf("Errore scrittura su pipe tmp da parte del primo figlio\n");
					exit(5);
				}
				nw = write(piped[0][1],&tmp2,sizeof(tmp2));
				if(nw != sizeof(tmp2))
				{
					printf("Errore scrittura su pipe tmp2 da parte del primo figlio\n");
					exit(5);
				}

			}
			else
			{
				//Leggo dal i-esimo figlio..
				nr = read(piped[i-1][0],&tmp,sizeof(tmp));
				if(nr != sizeof(tmp))
				{
					printf("Errore lettura tmp da pipe da parte del %d figlio\n",i);
					exit(6);
				}
				nr = read(piped[i-1][0],&tmp2,sizeof(tmp2));
				if(nr != sizeof(tmp2))
				{
					printf("Errore lettura tmp2 da pipe da parte del %d figlio\n",i);
				}

				if(occor > tmp.c1)
				{
					tmp.c1=occor;
					tmp.c2=i;
					tmp.c3=tmp.c3+occor;
					tmp2.pidd=getpid();
				}
				else
				{
					tmp.c3=tmp.c3+occor;
				}

				nw = write(piped[i][1],&tmp,sizeof(tmp));

				if(nw != sizeof(tmp))
				{
					printf("Scrittura da parte del %d figlio di tmp fallita sulla pipe\n",i);
					exit(8);
				}

				nw = write(piped[i][1],&tmp2,sizeof(tmp2));

				if(nw != sizeof(tmp2))
				{
					printf("Scrittura da parte del %d figlio di tmp2 fallita sulla pipe\n",i);
					exit(9);
				}

			}


			exit(i);
		}
	}

	//caso del padre
	if (pid > 0)
	{

		printf("Sono il padre di pid %d e sono partito..\n",getpid());

		//Chiudo le pipe che non mi servono
		//Siccome il padre ha solo il compito di leggere dall'ultimo figlio
		//	In SCRITTURA chiudo tutto
		//	In LETTURA chiudo tutto lasciando aperta l'ultima pipe per la lettura

		for(i=0; i<N; i++)
		{
			close(piped[i][1]);
			if(i != N-1)
				close(piped[i][0]);
		}

		//Leggo nella struct l'ultima info ricevuta
		nr = read(piped[N-1][0],&tmp,sizeof(tmp));
		if(nr != sizeof(tmp))
		{
			printf("PRIMA struttura non letta correttamente dal padre\n");
			exit(5);
		}

		nr = read(piped[N-1][0],&tmp2,sizeof(tmp2));
		if(nr != sizeof(tmp2))
		{
			printf("SECONDA struttura non letta correttamente dal padre\n");
			exit(6);
		}

		printf("Il valore massimo di occorrenze e' '%ld' l'indice e' '%d' e la somma di tutte le occorrenze e' '%ld' e il pid e' '%d' e il nome del file e' '%s'\n",tmp.c1,tmp.c2,tmp.c3,tmp2.pidd,argv[tmp.c2+1]);


		for(i=0; i<N; i++)
		{
			wait(NULL);
		}

		exit(0);
		                                                              
	}

















	
	
}