#!/bin/sh

#chiamata ricorsiva

for fname in *
do
    if test -d $fname -a -x $fname; then
    $0 $fname $2 $3
    fi
done

#controlli del caso
case $1 in
  */$2) for fname in *;
        do
          if test -f $fname -a -r $fname; then
          num_li=`wc -l < $fname`
            #se il numero di linee <5
            if test $num_li -lt 5; then
             >$fname.NOquintultima
             `pwd`/$fname.NOquintultima >> $3
            else
              linea=tail -5 $fname | head -1
              >$fname.quintultima
              echo $linea >> $fname.quintultima
              echo `pwd`/$fname.quintultima >> $3
            fi
          fi
        done
      ;;
  *);;
esac



exit 0
