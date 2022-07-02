#!/bin/sh

#controllo il numero dei parametri
if test $# -ne 3; then
  echo Numero parametri sbagliato
  exit 1
fi

#controllo primo parametro
case $1 in
/*) 	if test ! -d $1 -o ! -x $1
	then
	echo "Direttorio $1 inesistente o inaccessibile"
	exit 2
	fi;;
*) 	echo "Nome $1 non assoluto"
	exit 3;;
esac

# controllo secondo parametro
case $2 in
*[!0-9]*) 	echo Secondo parametro $2 non numerico o non positivo
 	    	exit 4;;
*)		if test $2 -eq 0
		then 	echo Secondo parametro $2 non strettamente maggiore0
                	exit 5
		fi;;
esac

# controllo terzo parametro
case $3 in
?);;
*)
echo "Terzo parametro $3 non singolo carattere"
exit 6;;
esac

# impostazione di PATH
PATH=$PATH:`pwd`
export PATH

parteR.sh $1 $2 $3


exit 0
