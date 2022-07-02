

fd=open(argv[i+1], O_RDWR)
O_RDONLY
O_WRONLY
fd = open(tmpname, O_WRONLY | O_APPEND | O_CREAT, 0644);
lseek(fd, -1L, SEEK_CUR);




/*Primo parametro singolo carattere CZ*/
	if(strlen(argv[1]) != 1)
	{
		printf("Il parametro non e' un singolo carattere\n");
		exit(1);
	}

	char CZ = argv[1][0];

/* parte in C accetta un numero variabile pari 2N di parametri maggiore o uguale a 2 (da controllare) che
rappresentano N nomi assoluti di file F1, ... FN intervallati da numeri interi strettamente positivi X1, X2, ... XN
che rappresentano la lunghezza in linee dei file (si può supporre che i parametri di posizione pari siano numeri e
si deve solo controllare che siano strettamente positivi)*/
	
	//Controllo numero di parametri
	if(argc < 3 || (argc-1)%2)
	{
		printf("Numero di parametri sbagliato\n");
		exit(1);
	}

	int N = argc-1;

	//Controllo che i parametri in posizione pari siano numeri > 0
	for(i=0; i<argc; i++)
	{
		if(i==0)
			continue;
		if(i%2==0)
		{
			int tmp=atoi(argv[i]);
			if(tmp <= 0)
			{
				printf("Numero negativo!\n");
				exit(2);
			}
		}
	}

	N=N/2;

	sprintf(opzione, "-%d", toRead);
	//Per aprire poi il file di competenza
	execlp("head", "head", opzione, argv[(i*2)+1], (char *)0);
