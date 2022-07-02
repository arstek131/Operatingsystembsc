#!/bin/sh

#controllo il numero dei parametri

if [ $# -ne 2 ]; then
  echo "Numero di parametri sbagliato"
  exit 1
fi

#controllo che il direttorio sia assoluto $1
case "$1" in
  /*) ;;
  *) echo "Forma assoluta pls"
     exit 2
     ;;
esac

#controllo che si tratti di un direttorio e che sia traversabile

if [ ! -d "$1" -o ! -x "$1" ]; then
  echo "Passare un direttorio oppure non e' traversabile"
  exit 3
fi

#controlli sul 2 parametro

#controllo che abbia nome relativo semplice

case "$2" in
  */*) echo "Pls direttorio in forma relativa semplice"
       exit 4
       ;;
  *) ;;
esac

export PATH=$(pwd):$PATH
es41r.sh $* A #invio questo parametro per identificare in quale fase sono

echo "Creare i direttori rimanenti? (y/n)"
read risposta

if [ "$rispota" == "y" ]; then
    es41r.sh $* B
    es41r.sh $* A #richiamo questa fase per la ristampa
fi

exit 0




