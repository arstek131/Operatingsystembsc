#!/bin/bash

if [ $# -ne 1 ]; then
  echo "max 1 argoemento"
  exit 1
fi

case "$1" in
  /*)
    echo "percorso assoluto"
    exit 0
    ;;
  */*) echo "percorso relativo"
   exit 0
   ;;
  *) echo "percorso semplice relativo"
   exit 0
   ;;
esac
