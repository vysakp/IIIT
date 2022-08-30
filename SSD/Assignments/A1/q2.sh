#!/bin/bash


# Bash program to generate a password with the following constrain

	# a. Password length should not be less than 6 characters
	# b. Password should have at least one upper case alphabet, one lowercase alphabet, one symbol, one number
	# c. Password should not be a palindrome
	# d. Password should not have a lowercase alphabet and a number together in any order
	# e. Password should not be greater than 20 characters
	# f. Password should not start with a symbol
	# g. Password should not have two symbols together
	# h. Password can start with an uppercase alphabet or a lowercase alphabet
	# i. Password should not end with an uppercase alphabet
	# j. Password should not have repetitive lowercase alphabets 



# check_plaindrome() {
# string=$1
# slen=$(( ${#string}-1 ))
# for(( i=0;i<=$slen/2;i++ ))
# do
# echo ${string:$i:1} ${string:$i-1:1}
# if [[ ${string:$i:1}!=${string:$slen-$i:1} ]]
# then
# return 0
# fi
# done
# return 1
# }


start=true

while $start
do
# taking a random length for password between 6 and 20 | (a) and (e)
len=$(( $RANDOM%15+6 ))

# generating random string of desired length  
# pass=$(openssl rand -base64 32)
# pass=${pass:0:$len}

pass=$(LC_ALL=C tr -dc 'A-Za-z0-9!"#$%&'\''()*+,-./:;<=>?@[\]^_`{|}~' </dev/urandom | head -c $len)


#checking condition (b)
if ! grep -E ".*[a-z].*" <<< $pass | grep -E ".*[A-Z].*" | grep -E ".*\\d.*" | grep -qE ".*\\W.*"; then
  continue
fi

#checking condition (d)
if ! grep -Ev ".*[a-z][0-9].*" <<< $pass | grep -qEv ".*[0-9][a-z].*"; then
  continue
fi

#checking condition (f) (h)
if ! grep -qEv "^[^a-zA-Z].*"<<< $pass; then
  continue
fi

#checking condition (g)
if ! grep -qEv ".*[^a-zA-Z0-9][^a-zA-Z0-9].*"<<< $pass; then
  continue
fi

#checking condition (i)
if ! grep -qEv ".*[A-Z]$"<<< $pass; then
  continue
fi

#checking condition (j)
if ! grep -Eo "[a-z]" <<< $pass |sort| tr -d '\n'|grep -Evq "([a-z])\1+"; then
  continue
fi
start=false
done

echo $pass
