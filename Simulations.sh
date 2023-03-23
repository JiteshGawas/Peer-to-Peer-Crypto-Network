#!/bin/bash

if [ $# -ne 5 ]; then
  echo "Usage: $0 numpeers z0 z1 advTao advType"
  exit 1
fi

numpeers=$1
z0=$2
z1=$3
advTao=$4
advType=$5

for i in {2..10}
do
  advMin=$(echo "scale=1; $i/10" | bc)
  echo "Executing command with advMin=$advMin"
  ./Build/main $numpeers $z0 $z1 $advMin $advTao $advType >> out.txt
done
