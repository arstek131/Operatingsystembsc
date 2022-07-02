#!/bin/bash

USAGE="usage: $0 dirname(abs)"

#controllo num parametri
if [ $# -ne 1 ]; then
    echo "$USAGE"
    exit 1
fi

#controllo se parametro e' nome assoluto
case "$1" in
  /*) ;;
  *) echo "$USAGE"
     exit 1;;
esac

#controllo se si tratta di un direttorio e che sia accessibile
if [ ! -d "$1" -o ! -x "$1" ]; then
    echo "$USAGE"
    exit 1
fi

#ridirigo il livello massimo sul file maxlevel in tmp
echo "0" > /tmp/maxlevel
export PATH=$(pwd):$PATH
es31r.sh "$1" 0
echo "massima profondita: $(cat /tmp/maxlevel)"

exit 0





