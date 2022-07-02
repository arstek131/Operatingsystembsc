#!/bin/bash

if [ $# -ne 2 ]; then
  echo "il numero di argomenti non e' 2"
  exit 1
fi

if [ ! -d "$1" -a ! -w "$1" ]; then
  echo "La directory non e' esiste oppure non e' scrivibile"
  exit 1
fi

cd $1
pwd > $2

exit 0
