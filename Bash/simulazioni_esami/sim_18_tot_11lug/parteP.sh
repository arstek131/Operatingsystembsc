#!/bin/sh

#controllo il numero dei parametri
if test $# -le 2; then
  echo Numero parametri sbagliato
  exit 1
fi

#controllo il primo parametro
case $1 in
  ?) ;;
  *) echo Solo un carattere plz
     ;;
esac

tmp=$1
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

>/tmp/localFile

for i
do
  parteR.sh $i $tmp /tmp/localFile
done

num_li=`wc -l < /tmp/localFile`

echo In totale sono stati trovati $num_li file

rm -rf /tmp/localFile



exit 0

