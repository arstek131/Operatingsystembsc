#!/bin/sh

#controllo il numero dei parametri
if test $# -ne 2; then
  echo Numero paremetri sbagliato
  exit 1
fi

#controllo che si tratti di una directory e che sia assoluta
case $1 in
	/*) if test ! -d $1 -o ! -x $1
	    then
	    echo $1 non directory
	    exit 2
	    fi;;
	*)  echo $1 non nome assoluto; exit 3;;
esac

#controllo che si tratti di un numero e che sia >=2
expr $2 + 0  > /dev/null 2>&1
if test $? -ne 2 -a $? -ne 3
  then
     if test $2 -le 1
       then echo $2 non conforme
          exit 4
     fi
else
  echo $2 non numerico
  exit 5
fi

tmp=`expr $2 \* 2`

#controlli sui parametri finiti possiamo passare alle N fasi
PATH=`pwd`:$PATH
export PATH

FCR.sh $1 $tmp



exit 0
