#!/bin/sh

#controllo dei parametri

if test $# -le 2; then
  echo Numero parametri sbagliato
  exit 1
fi

#controlli sul primo parametro, che sia un nome relativo semplice di D
case $1 in
  */*) echo Nome relativo semplice pls
       exit 2
       ;;
  *) ;;
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
	    exit 3
	    fi;;
	*)  echo $i non nome assoluto; exit 4;;
	esac
done

#ho finito il controllo sui parametri mi preparo a lanciare le N fasi

export PATH=$(pwd):$PATH


>/tmp/filetmp

for i
do
  parteR.sh $i $tmp /tmp/filetmp
done

linee_tot=`wc -l < /tmp/filetmp`

echo Sono stati creati $linee_tot file globalmente

for i in `cat /tmp/filetmp`
do
  case $i in
    *.quintultima) echo Stai per visualizzare il contenuto del file $i
                   cat $i
                   ;;
    *) echo Purtroppo il file $i e vuoto
        ;;
  esac
done


rm -rf /tmp/filetmp


exit 0
