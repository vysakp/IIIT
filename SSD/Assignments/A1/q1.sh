#!/bin/bash


# Fuction to get the digital root of the twin prime product 

# If either of the number is 3 then the digital root of the twin primes will always be 8
# # # # Proof 
# Of any 3 consecutive integers, one is divisible by 3. If 2 of the integers are a pair of twin primes
# neither of which is 3, then neither of the "end" numbers is divisible by 3. So the "middle" number must
# be divisible by 3.

# So the twin primes are 3𝑘−1 and 3𝑘+1 for some integer 𝑘. It follows that their product is 9𝑘2−1=9𝑘2−9+8.
# We conclude that the remainder when (3𝑘−1)(3𝑘+1) is divided by 9 is 8.

# But the remainder when a number 𝑛 is divided by 9 is the same as the remainder when the sum of the decimal
# digits of 𝑛 is divided by 9. Thus the digital root of our product is 8.
# Hence except for 3*5 all the twin primes will have digital root as 8
digital_root () {
 prod=$1
 sum=0
 if [ $prod -eq 15 ]; then 
    return 6
 else 
    return 8
 fi
}


echo "Enter the number"
read num

# Fuction to get the prime numbers until the given number using SieveOfEratosthenes method

declare -a sieve[num] 
SieveOfEratosthenes(){
        for((i=0;i<num;i++)){
                sieve[i]=1
        }
        for((i=1;i<num/2;i++))
        do
                if [ ${sieve[$i]} ]; then
                        val=$(($i+1))
                        for((j=i+val;j<num;j+=val))
                        do
                                sieve[j]=0
                        done
                fi
        done
        


}


if [ $num -ge 0 ];then
        SieveOfEratosthenes
        echo "Number is $num"
        prev=1
        out=0
        for((i=2;i<num;i++))
        do
                if [ ${sieve[$i]} -eq 1 ]; then
                        diff=$(( $i-$prev ))
                        # echo $i $prev $diff 
                        if [ $diff -eq 2 ]; then
                                echo "($((prev+1)), $((i+1)))"
                                prod=$(( ($prev+1)*($i+1) ))
                                digital_root $prod
                                out=$(($out+$?))
                        fi
                        prev=$i
                fi
        done
        echo "Final output=$out"

else
        echo "Number cannot be negative"
fi

