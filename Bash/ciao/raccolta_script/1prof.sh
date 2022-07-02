#!/bin/sh
#Poco significativo se non si passano parametri (meglio 4)
echo I PARAMETRI
echo '$1 ===>' $1
echo '$2 ===>' $2
echo '$3 ===>' $3
echo '$4 ===>' $4
echo '$* (TUTTI I PARAMETRI, a parte $0) ===>' $*
echo '$# (NUMERO DEI PARAMETRI) ===>' $#
shift
echo DOPO LO SHIFT
echo '$1 ===>' $1
echo '$2 ===>' $2
echo '$3 ===>' $3
echo '$* (TUTTI I PARAMETRI, a parte $0) ===>' $*
echo '$# (NUMERO DEI PARAMETRI) ===>' $#
ls script.sh
echo '$? (VALORE DI RITORNO ULTIMO COMANDO) ===>' $?
ls pippo
echo '$? (VALORE DI RITORNO ULTIMO COMANDO) ===>' $?
echo '$$ (NUMERO DEL PROCESSO IN ESECUZIONE) ===>' $$
