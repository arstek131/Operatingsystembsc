#!/bin/bash

#se il numero di argomenti passati è < 1

if [ $# -lt 1 ]; then
  echo "Il numero di argomenti passati e <1"
  exit 1
fi

#finchè il numero dei parametri è > 1 continua a shiftare

while [ $# -gt 1 ]; do
  shift
done

#stampo l'ultimo parametro
echo "$1"
exit 0

