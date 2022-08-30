#!/bin/bash

#Getting the file name
file=$1

# Getting the total number of lines in the file
count=$(wc -l $file) 
count=$(cut -d " " -f 1 <<< $count)

#getting the middle line number
if [[ $count%2 -eq 1 ]]; then odd=1; else odd=0; fi 
count=`expr $count + $odd`
count=`expr $count / 2`


#printing the middle line 
awk -v count="$count" '{if(NR==count) print $0}' $file


