#!/bin/bash
rm -f out
ret=0
for inFile in *.in; do
  prefix=${inFile::-3}
  outFile=$prefix".exp"
  ../jay $inFile out
  if ! cmp out $outFile >/dev/null 2>&1
  then
    echo "failure on $prefix"
    ret=1
  fi
  rm -f out
done
exit $ret
