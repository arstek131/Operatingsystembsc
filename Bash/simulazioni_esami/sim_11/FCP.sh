#!/bin/sh

#controllo il numero dei parametri
if test $# -ne 3; then
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

#Controllo che si tratti di un nome relativo semplice
case $2 in
*/*) echo Errore: $2 non in forma relativa semplice
    exit 4;;
*) ;;
esac

#controllo che si tratti di un numero e che sia >0
expr $3 + 0  > /dev/null 2>&1
if test $? -ne 2 -a $? -ne 3
  then
     if test $3 -le 0
       then echo $3 non positivo
          exit 5
     fi
else
  echo $3 non numerico
  exit 6
fi

#controlli sui parametri finiti possiamo passare alle N fasi
PATH=`pwd`:$PATH
export PATH

> /tmp/conta$$

FCR.sh $1 $2 $3 A /tmp/conta$$

NF=`wc -l < /tmp/conta$$`

echo Il numero totale di file che soddisfano e $NF

if test $NF -lt $3; then
  echo Passo alla fase B
  FCR.sh $1 $2 $3 B /tmp/conta$$
  > /tmp/conta$$
  echo Passo alla fase C
  FCR.sh $1 $2 $3 A /tmp/conta$$

  NF=`wc -l < /tmp/conta$$`

  echo Il numero totale di file che soddisfano e $NF
else
    rm /tmp/conta$$
    exit 7
fi




rm /tmp/conta$$

exit 0
