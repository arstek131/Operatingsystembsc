#!/bin/sh
#FCR.sh
#file comandi ricorsivo che scrive il nome dei direttori trovati sul file temporaneo
#il cui nome e' passato come terzo argomento

cd $1

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio
		$0 `pwd`/$i $2 $3
	fi
done


#controlli del caso
flag=0
num_li=0
case $1 in
*/$2)  for i in *
       do
           if test -f $i -a -r $i; then
             num_li=`wc -l < $i`
             if test $num_li -eq $3; then
               flag=1
             fi
           fi
        done
        if test $flag -eq 1; then
          echo `pwd`
        fi
           ;;
*) ;;
esac

exit 0
