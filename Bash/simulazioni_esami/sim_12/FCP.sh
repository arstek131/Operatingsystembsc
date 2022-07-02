#!/bin/sh

#controllo il numero dei parametri
if test $# -ne 1; then
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
	*)  echo $i non nome assoluto; exit 3;;
esac

PATH=`pwd`:$PATH
export PATH

> /tmp/maxlevel$$
echo 0 > /tmp/totLevel$$


FCR.sh $1 A 1 /tmp/totLevel$$

num_li=`wc -l < /tmp/totLevel$$`
echo Il numero totale di livelli e $num_li




rm -rf /tmp/totLevel$$



exit 0
