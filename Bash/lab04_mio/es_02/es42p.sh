#!/bin/sh

if test $# -ne 3; then
  echo "Numero parametri sbagliato"
  exit 1
fi

#controllo che sia assoluto

case "$1" in
  /*);;
  *) echo "Direttorio assoluto pls"
     exit 2
     ;;
esac

if [ ! -d "$1" -o ! -x "$1" ]; then
  echo "Non e' un direttorio o non e' traversabile"
  exit 3
fi

#controllo che il secondo parametro sia un nome relativo

case "$2" in
  */*) echo "Nome relativo semplice pls"
       exit 4
       ;;
  *) ;;
esac

#controllo che il terzo parametro sia un numero >0

#controllo che si tratti di un numero

expr $3 + 0

if test $? ! -eq 0; then
  echo "Solo numeri per favore"
  exit 5
fi

#controllo che sia >0

if test $3 -le 0; then
  echo "Per favore solo numeri positivi"
  exit 6
fi

rm -rf /tmp/flist >/dev/null 2>&1

export PATH=$(pwd):$PATH
es42r.sh $*

conto=0
if [ -f "/tmp/flist" ]; then
    conto=$(wc -l < /tmp/flist)
fi

echo "File trovati $conto"

exit 0









