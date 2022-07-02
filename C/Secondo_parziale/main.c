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

typedef int pipe_t[2];

typedef struct
{
   int c1;
   int c2;
   char c3[250];
}s_occ;

int main(int argc, char const *argv[])
{
///////////////////////////////////////////////////////////////////////////////	
	//Variabile usate per controllare il valore di ritorno del Figlio e Nip
	int pidFiglio, pidNipote;
	int status;
	//Variabile usata per salvarmi il valore di ritorno dei figli e nipoti
	int ritorno;
	//Variabile usata per conservare la riga e le altre info
	s_occ tmp;
	//Variabile usata per salvarmi la lunghezza della linea
	int k;
	//Variabile usate per controllare il ritorno di write e read su pipe
	int nw, nr;
	//Pipe usata per la comunicazione Figlio-Nipote
	pipe_t new;
	//Variabile usata per salvare il pid di nipoti e figli
	int pid;
	//Variabili usate nei cicli
	int i,j;
	//Array di pipe usati per la comunicazione Padre-Figlio
	pipe_t *piped;
	//Variabile che contiene il numero dei file
	int Z;
///////////////////////////////////////////////////////////////////////////////
	//Controllo il numero di parametri
	if(argc < 4)
	{	
		printf("Numero di argomenti sbagliato\n");
		exit(1);	
	}
	Z=argc-1;
	//Alloco l'array di pipe per la comunicazione Padre-Figlio
	piped=(pipe_t*)malloc(Z*sizeof(pipe_t));
	if(piped==NULL)
	{
		printf("Allocazione array di pipe fallita\n");
		exit(2);
	}
	//Creo le pipe
	for(i=0; i<Z; i++)
	{
		if(pipe(piped[i])<0)
		{
			printf("Chiamata a funzione pipe() fallita\n");
			exit(3);
		}
	}

	//Genero i Z figli
	for(i=0; i<Z; i++)
	{
		pid=fork();
		if(pid < 0)
		{
			printf("Chiamata a funzione fork() fallita\n");
			exit(5);
		}	
		//Caso del figlio
		else if (pid==0)
		{
		    printf("Sono il figlio di indice %d e pid %d\n",i,getpid());
		    //Chiudo le pipe che non uso
		    for(j=0; j<Z; j++)
		    {
			close(piped[j][0]);
			if(j != i)
			{
			  close(piped[j][1]);
			}
                    }	  	
		    
		    //Creo la pipe di comunicazione tra Figlio-Nipote
		    if(pipe(new)<0)
		    {
			printf("Creazione pipe Figlio-Nipote fallita\n");
			exit(-1);
 		    }	
		   
		    //Creo il nipote
		    pid=fork();
		    if(pid<0)
		    {
			printf("Chiamata a funzione fork() fallita nipote\n");
			exit(-1);
		    }
			
		  //Caso del nipote
		  if(pid == 0)
		  {
		     printf("Sono il nipote di pid %d\n",getpid());

		     //Chiudo la pipe di comunicazione tra Padre-Figlio
		     close(piped[i][1]);
		     //Chiudo lo stdin
		     close(0);
		     //Apro il file di competenza
		     if(open(argv[i+1],O_RDONLY)<0)
		     {
			printf("Errore apertura %s\n",argv[i+1]);
			exit(-1);
		     }  
		     //Chiudo stdout e lo redirigo
		     close(1);
		     dup(new[1]);
		
		     //Ora il nipote può chiudere entrambi i lati della pipe
		     //Il lato 0 non viene usato
		     //Il lato 1 viene usato da stdout
		     close(new[0]);
		     close(new[1]);

		     //Redirigo lo stderror su dev/null
		     close(2);
		     open(" /dev/null",O_WRONLY);

		     //Eseguo il comando sort
	             execlp("sort", "sort", (char *)0);
			
		    //Qua non ci dovrebbe mai arrivare
		    exit(-1);		
  		  }	
		  //Torno nel figlio
		  //Chiudo la pipe in scrittura tra figlio nipote
		  close(new[1]);

	          //Leggo dalla pipe Figlio-Nipote esclusivamente
	          //La 1 linea

		  k=0;
		  while(read(new[0],&tmp.c3[k],1)==1)
		  {
		      if(tmp.c3[k] == '\n')
			  break;
		      k++;
		  }

		  tmp.c1=pid;
		  tmp.c2=k+1; //per il terminatore
	
		  //Il figlio comunica al padre la struttura
		  nw = write(piped[i][1],&tmp,sizeof(tmp));
		  if(nw != sizeof(tmp))
		  {
		     printf("Scrittura fallita indice %d pid %d\n",i,getpid());
		     exit(-1);
		  }
		
		  //Il figlio aspetta il nipote
		  //Se il nipote ritorna 0 significa che è andata a buon fine
		  ritorno = -1;
		  pidNipote = wait(&status);
		  if(pidNipote < 0 )
		  {
			printf("Errore wait() nel figlio\n");
			exit(-2);
		  }
		  if((status & 0xFF) != 0)
			printf("Nipote con pid %d terminato in modo anomalo\n",pidNipote);
		else
			printf("Il nipote di pid=%d ha tornato %d\n",pidNipote,ritorno=(int)((status >> 8) & 0xFF));


		
		  exit(k);
			
		}

	}//Fine del ciclo for
	
	//Codice del padre
	printf("Sono il padre di pid %d\n",getpid());
	
	//Chiudo le pipe che non uso nei confronti del figlio
	for(i=0; i<Z; i++)
	{
	   close(piped[i][1]);
	}

	//Il padre legge le strutture in ordine
	for(i=0; i<Z; i++)
	{	
	   nr = read(piped[i][0],&tmp,sizeof(tmp));
	   if(nr != sizeof(tmp))
	   {
		printf("Lettura fallita da pipe da parte del padre %d",i);
		exit(6);
	   }		
	   //Trasformo in stringa il campo c3 di ogni struttura
	   //Aggiungendoci il carattere terminatore
	   tmp.c3[tmp.c2-1]=0;
	   printf("Nipote con pid %d ha letto stringa di lunghezza %d , stringa:'%s' per il file %s\n",tmp.c1,tmp.c2,tmp.c3,argv[i+1]);	
		
	}
	//Il padre aspetta i figli
	for(i=0; i<Z; i++)
	{
	   pidFiglio = wait(&status);
	   if(pidFiglio < 0)
	   {
		printf("Errore wait\n");
		exit(7);
	   }
	   if((status & 0XFF) != 0)
		printf("Figlio con pid %d terminato in modo anomalo\n",pidFiglio);
	   else
	   {
		ritorno = (int)((status >> 8) & 0xFF);
		printf("Il figlio con pid= %d ha tornato %d\n",pidFiglio,ritorno);
	   }
	}






	exit(0);

	

}
