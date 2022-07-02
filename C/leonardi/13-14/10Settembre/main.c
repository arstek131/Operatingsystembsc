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
	int index;
	int len;
	
}info;

void BubbleSort(info *array, int elemN)
 {
   int i, ultimo;
   info tmp;
   int alto=elemN; /* elemN è il numero degli elementi del vettore da ordinare */
   ultimo=alto;
   
   while (alto >= 0) /* in questo modo si evita 1 passaggio*/
     { 
         ultimo = -1;
         for (i=0; i<alto-1; i++) /* si mostra che un semplice array come 
                                     int nums[elemN] = {4,3,2,1};
                                     non viene ordinato correttamente a causa della condizione i<alto-1
                                  */
         {
           if (array[i].len>array[i+1].len) /* sostituire ">" con "<" per avere un ordinamento decrescente */
           { 
             tmp = array[i]; 
             array[i] = array[i+1]; 
             array[i+1] = tmp;
             ultimo = i;
           } 
         }
     alto = ultimo;
     }
 }


int main(int argc, char const *argv[])
{
	pipe_t *pf, *fp;
	int *pid;
	int i,j;
	char line[255];
	int k=0;
	int fd;
	int linea=0;
	int nw,nr;
	
	//Controllo il numero di parametri
	if(argc < 4)
	{
		printf("Numero di argomenti sbagliato\n");
		exit(1);
	}

	int K = atoi(argv[argc-1]);

	if(K <= 0)
	{
		printf("L'ultimo parametro deve essere >0\n");
		exit(2);
	}

	int X;

	printf("Inserisci il numero X >0 e < di %d\n",K);
	scanf("%d",&X);

	if(X <=0 || X > K)
	{
		printf("Numero X inserito non conforme\n");
		exit(3);
	}

	int N = argc-2;

	//Creo gli array di pipe
	pf = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(pf == NULL)
	{
		printf("Allocazione dell'array pf fallita\n");
		exit(4);
	}


	fp = (pipe_t *)malloc(N*sizeof(pipe_t));
	if(fp == NULL)
	{
		printf("Allocazione dell'array fp fallita\n");
		exit(4);
	}

	for(i=0; i<N; i++)
	{
		if((pipe(pf[i]) < 0) || (pipe(fp[i])<0))
		{
			printf("Chiamata a funzione pipe() fallita\n");
			exit(5);
		}
	}

	//Creazione dell'array di pid
	pid = (int *)malloc(N*sizeof(int));
	if(pid == NULL)
	{
		printf("Allocazione arrayd di pid fallita\n");
		exit(6);
	}

	//Generazione dei figli
	for(i=0; i<N; i++)
	{
		pid[i]=fork();
		if(pid[i]<0)
		{
			printf("Chiamata a funzione fork() fallita\n");
			exit(7);
		}
		//Caso del figlio
		if(pid[i] == 0)
		{
			printf("Sono il figlio di indice %d e pid %d e sono partito\n",i,getpid());
			//Chiudo le pipe che non uso
			for(j=0; j<N; j++)
			{
				close(pf[j][1]);
				close(fp[j][0]);
				if(i != j)
				{
					close(pf[j][0]);
					close(fp[j][1]);
				}
			}

			fd=open(argv[i+1],O_RDONLY);
			if(fd < 0)
			{
				printf("Apertura del file %s fallita\n",argv[i+1]);
				exit(-1);
			}

			k=0;
			linea=1;
			info istanza;
			while(read(fd,&line[k],1)==1)
			{

				if(line[k] == '\n')
				{
					if(linea == X)
					{
						break;
						/*k++; //per il terminatore
						//Linea trovata, la comunico al padre
						//k mi rappresenta la lunghezza della linea
						//printf("La linea %d del file %s ha lunghezza %d\n",linea,argv[i+1],k);
						nw = write(fp[i][1],&k,sizeof(k));
						if(nw != sizeof(k))
						{
							printf("Errore scrittura su pipe da parte del figlio di indice %d\n",i);
							exit(-1);
						}

						nr=read(pf[i][0],&istanza,sizeof(istanza));
						if(nr != sizeof(istanza))
						{
							printf("Seconda lettura fallita da pipe da parte del figlio\n");
							exit(-1);
						}
						if(istanza.index == i)
						{
							line[k-1]=0;
							printf("Sono il figlio di indice %d e la mia %d linea per il file %s e' %s\n",i,X, argv[i+1],line);
						}
						*/

					}

					k=0;
					linea++;
				}
				else
				{
					k++;
				}
			}

			k++;
			nw = write(fp[i][1],&k,sizeof(k));
			if(nw != sizeof(k))
			{
				printf("Errore scrittura su pipe da parte del figlio di indice %d\n",i);
				exit(-1);
			}

			nr=read(pf[i][0],&istanza,sizeof(istanza));
			if(nr != sizeof(istanza))
			{
				printf("Seconda lettura fallita da pipe da parte del figlio\n");
				exit(-1);
			}
			if(istanza.index == i)
			{
				line[k-1]=0;
			   	printf("Sono il figlio di indice %d e la mia %d linea per il file %s e' %s\n",i,X, argv[i+1],line);
			 }



			exit(0);
		}	
	}
	//Caso del padre
	printf("Sono il padre di pid %d e sono partito\n",getpid());

	//Chiudo le pipe che non uso
	for(i=0; i<N; i++)
	{
		close(fp[i][1]);
		close(pf[i][0]);
	}

	info *tmp;

	tmp = (info *)malloc(N*sizeof(info));
	if(tmp == NULL)
	{
		printf("Allocazione array di info fallita\n");
		exit(8);
	}

	//Ricevo le varie lunghezze dai figli
	k=0;
	for(i=0; i<N; i++)
	{
		nr = read(fp[i][0],&k,sizeof(k));
		if(nr != sizeof(k))
		{
			printf("Lettura fallita da parte del padre\n");
			exit(9);
		}
		tmp[i].len=k;
		tmp[i].index=i;
	}

	//Ora applico un ordinamento all'array
	BubbleSort(tmp,N);

	for(i=0; i<N; i++)
	{
		printf("La lunghezza %d si riferisce al file %s\n",tmp[i].len,argv[tmp[i].index+1]);
	}

	//Comunico ai figli
	for(i=0; i<N; i++)
	{
		nw = write(pf[i][1],&tmp[i],sizeof(tmp));
		if(nw != sizeof(tmp))
		{
			printf("Scrittura su pipe fallita da parte del padre per il %d figlio\n",i);
		}
		sleep(1);

	}





	for(i=0; i<N; i++)
	{
		wait(NULL);
	}

	printf("Sono il padre di pid %d ed e' terminato tutto correttamente\n",getpid());
	exit(0);
}