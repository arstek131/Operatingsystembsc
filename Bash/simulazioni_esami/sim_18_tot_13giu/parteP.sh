#!/bin/sh

#controllo il numero dei parametri
if test $# -le 3; then
  echo Numero di parametri sbgliato
  exit 1
fi

#controllo che il primo parametro sia il nome relativo semplice di D
case $1 in
  */*) echo Nome relativo semplice pls
       exit 2
       ;;
  *) ;;
esac

#controllo che il secondo parametro sia un numero
expr $2 + 0 > /dev/null 2>&1

if test $? ! -eq 0; then
  echo "Solo numeri per favore"
  exit 3
fi

#controllo che sia >0

if test $2 -le 0; then
  echo Solo numeri positivi per favore
  exit 4
fi

tmp_dir=$1
shift
tmp_num=$1
shift

#controllo che i parametri rimanenti siano nomi assoluti di directory
for i
do
    case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
	    echo $i non direttorio
	    exit 5
	    fi;;
	*)  echo $i non nome assoluto; exit 6;;
	esac
done

#ho finito il controllo sui parametri mi preparo a lanciare le N fasi

export PATH=$(pwd):$PATH


for i
do
  >/tmp/contoLocal
  parteR.sh $i $tmp_dir $tmp_num /tmp/contoLocal
  num_li=`wc -l < /tmp/contoLocal`
  echo Per la gerarchia $i sono stati trovati $num_li file
  rm -rf /tmp/contoLocal
done

rm -rf /tmp/contoLocal



exit 0
