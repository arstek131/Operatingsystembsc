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

echo Comincia la fase A
> /tmp/conta$$ #creiamo/azzeriamo il file temporaneo.

FCR.sh $1 $2 $3 /tmp/conta$$

num_li=`wc -l < /tmp/conta$$`
echo Il numero totale di direttori che soddisfano sono $num_li
cat /tmp/conta$$

echo Passo alla fase B

if test $num_li -gt $3; then
  X=0
  echo Inserisci un numero compreso tra 1 e $num_li
  read X
    #controllo che si tratti di un numero e che sia compreso tra 1 e N
    expr $X + 0  > /dev/null 2>&1
    if test $? -ne 2 -a $? -ne 3
    then
      if test $X -le 0 -o $X -gt $num_li
       then echo $X non conforme
          exit 7
      fi
    else
      echo $X non numerico
      exit 8
    fi
    cazzo=`head -$X /tmp/conta$$ | tail -1`
    echo Direttorio selezionato $cazzo
    cd $cazzo
    for fname in *; do
      if test -f $fname -a -r $fname; then
        case $fname in
        *.$2) head -1 $fname | tail -1
              ;;
        *) ;;
        esac
      fi
    done
fi



#da ultimo eliminiamo il file temporaneo
rm /tmp/conta$$

exit 0
