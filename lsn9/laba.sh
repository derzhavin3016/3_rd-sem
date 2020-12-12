#!/bin/bash
if [ $# -eq 0  ]
then
  echo Usage $0 NUMBER_TO_CALC
  exit 1
fi


for (( i = 1; i <= 25; i++))
do
  echo
  echo num = $i
  time ./th $1 $i 4
done
