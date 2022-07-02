*******************+FILE P************************
#!/bin/sh

#controllo il numero dei parametri
if test $# -ne 1; then
  echo Numero paremetri sbagliato
  exit 1
fi

#controllo che si tratti di una directory e che sia assoluta
case $1 in
	/*) if test ! -d $i -o ! -x $i
	    then
	    echo $1 non directory
	    exit 2
	    fi;;
	*)  echo $1 non nome assoluto; exit 3;;
esac

#controlli sui parametri finiti possiamo passare alle N fasi
PATH=`pwd`:$PATH
export PATH

> /tmp/conta$$ #file su cui mi salvo il livello massimo
echo 0 > /tmp/conta$$

echo Cominciola fase A
FCR.sh $1 0 0 A /tmp/conta$$

liv=`cat /tmp/conta$$`
echo Il numero totale di livelli di $1 e $liv


echo Passo alla fase B
echo Inserisci un numero pari compreso tra 1 e $liv
X=0
read X
#controllo che si tratti di un numero e che sia >0
expr $X + 0  > /dev/null 2>&1
if test $? -ne 2 -a $? -ne 3
  then
     if test $X -le 0
       then echo $X non conforme
          rm /tmp/conta$$
          exit 4
     fi
else
  echo $X non numerico
  rm /tmp/conta$$
  exit 5
fi

numero=`expr $X % 2`
if test $numero -ne 0; then
  echo Numero $X non pari
  rm /tmp/conta$$
  exit 6
fi

FCR.sh $1 0 $X B /tmp/conta$$



rm /tmp/conta$$
exit 0







*****************FILE R**************************
#!/bin/sh

cd $1

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamata ricorsiva cui passiamo come primo parametro il nome assoluto del direttorio
		FCR.sh `pwd`/$i `expr $2 + 1` $3 $4 $5
	fi
done

if test $4 = A; then
  #controlli del caso
  x=`cat $5` #mi salvo il livello attuale
  if test $2 -gt $x; then
    echo $2 > $5
  fi
fi

if test $4 = B; then
  if test $2 -eq $3; then
    pwd
    ls -la
  fi
fi


exit 0

