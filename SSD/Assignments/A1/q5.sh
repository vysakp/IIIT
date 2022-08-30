#!/bin/bash

echo "Enter the number"
read num
l0=2
l1=1
# echo 2 > l0.txt
# echo 1 > l1.txt
i=1
if [ $num -eq 0 ]; then echo $l0; 
elif [ $num -eq 1 ]; then echo $l1;
else
while [ $i -lt $num ]
do
	l2="$( bc <<< "$l0 + $l1" )"
	l0=$l1
	l1=$l2
	i=$((i+1))
done
echo $l2 
# grep -Eo "[^\\\ ]" <<<$l2 | tr -d '\n'
fi

#change index to include the 0th one 