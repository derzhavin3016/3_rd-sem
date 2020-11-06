#!/bin/bash

for (( i = 1; i <= 16; i++))
do
  echo
  echo num =  $i
  time ./th 100000 $i 1
done
