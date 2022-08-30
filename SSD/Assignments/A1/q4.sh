#!/bin/bash

echo "Enter the year"
read year
check_leap_year() {
year=$1
if [ `expr $year % 400` -eq 0 ];then
return 1
elif [ `expr $year % 100` -eq 0 ];then
return 0
elif [ `expr $year % 4` -eq 0 ];then
return 1
else
return 0
fi
}



#reversing the inputed yaer and taking the day and month
rev_year=$(echo $year|rev)
month=${rev_year:2:2}
day=${rev_year:0:2}

# mapping max days in each month
months=("31" "28" "31" "30" "31" "30" "31" "31" "30" "31" "30" "31")
check_leap_year $year
months[1]=`expr ${months[01]} + $?`




# stripping the leading zeros 
s_month=${month#0}
s_day=${day#0}


if [[ $s_month -gt 12 || $s_month -eq 0 || $s_day -eq 0 ]]; then
	echo "No Palindrome days available in the given year"
# echo ${months[$s_month-1]}
elif [[ $s_day -gt ${months[$s_month-1]} ]]; then
	echo "No Palindrome days available in the given year"
else 
	echo "$day-$month-$year"
fi




