#!/bin/sh

filetempo=/tmp/lista

cd "$1"

for fname in *; do
  if test -d "$fname" -a -x "$fname"; then
    "$0" "$fname"  $*
  fi
done

#cerco tutti i direttori che hanno nome D

if test $(basename $(pwd)) != "$2"; then
  exit 0
fi

#conto il numero dei sottodirettori di D

sottodir=0
for fname in *; do
  if test -d "$fname"; then
    sottodir=$(expr "$sottodir" + 1)
  fi
done

#ora conto il numero dei file che ha la directory
filess=0
for fname in *; do
  if test -d "$fname"; then
    filess=$(expr "$filess" + 1)
  fi
done

sottodir=$(expr "$sottodir" + 2)

if test "$sottodir" -eq "$filess"; then
  echo $(pwd)

sottodir=$(expr "$sottodir" - 2)

da_creare=$(expr "$filess" - "$sottodir")

diro="ciao"

if test "$da_creare" -gt 0; then
  while test "$da_creare" -eq 0; do
    echo "Inserisci il nome della dir da creare"
    read diro
    da_creare=$("$da_creare" - 1)
    mkdir "$diro"
    echo "testArs" >> $filetempo
  done
fi

exit 0





