#!/bin/bash

# defining Kaprekar's constant 
kap_const="6174"



start=true

#taking the input number making sure it is a four digit number other than 1111 or 0000
while $start
do
	echo "Enter a four digit number"
	read num
	if grep -Evq ^[0-9]{4}$ <<< $num; then
		echo -e "Please enter a 4 digit number\n"
		continue;
	fi
	if [[ $num == "1111" ||  $num == "0000" ]]; then
		echo -e "Please enter number other than 1111 or 0000\n"
		continue;
	fi
	start=false
done 


#Implementing Kaprekar’s routine and returning the output
while [ $kap_const -ne $num ]
do
	echo -n "$num, "
	num=$(grep -Eo . <<< $num | sort  | tr -d '\n') 
	rev=$(echo $num|rev)
	diff=`expr $num - $rev`
	# echo "num=$num rev=$rev diff=$diff"
	num=${diff#-}
done
echo $kap_const