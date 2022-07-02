#!/bin/bash

if [ $# -ne 1 ]; then
  echo "argomenti non sufficienti"
  exit 1
fi

if [ ! -d "$1" -o ! -r "$1" ]; then
  echo "file nome directory inesistente o non leggibile"
  exit 1
fi

exit 0
