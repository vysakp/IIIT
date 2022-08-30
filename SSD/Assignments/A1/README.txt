


############## q1.sh ############### 

Here for prime number calculation, we are using the Sieve of Eratosthenes method.

For digital root calculation, we are taking the digital root of (3,5) as 6 and 
remaining all the twin primes will be having digital root as 8
                
Proof: Of any 3 consecutive integers, one is divisible by 3. If 2 of the integers are a pair of twin primes
neither of which is 3, then neither of the "end" numbers is divisible by 3. So the "middle" number must
be divisible by 3.

So the twin primes are 3𝑘−1 and 3𝑘+1 for some integer 𝑘. It follows that their product is 9𝑘2−1=9𝑘2−9+8.
We conclude that the remainder when (3𝑘−1)(3𝑘+1) is divided by 9 is 8.

But the remainder when a number 𝑛 is divided by 9 is the same as the remainder when the sum of the decimal
digits of 𝑛 is divided by 9. Thus the digital root of our product is 8.

Execution: ./q1.sh

User is expected to provide the input for which the twin primes and output needs to be calculated 

Output format: Twin primes are printed in parenthesis (prime1, prime2), 
and the sum of the digital root of twin primes is given as the final output 


############## q2.sh ############### 

Here the condition (c)-Palindrome checking is avoided as with the remaining condition checked 
by script, the generated password can never be a palindrome 

Explanation: Password needs to start with an uppercase alphabet or a lowercase alphabet (h) + 
Password should not end with an uppercase alphabet (i) + Password should not have repetitive lowercase alphabets (j)
Here for condition (j), the script is checking for repetitive lowercase anywhere in the password, because of this added restriction password can never be a palindrome. 

Execution : ./q2.sh

Output format: Required password which is passing all the conditions is printed in the next line


############## q3.sh ############### 


Execution : ./q3.sh

Input is expected to be a four-digit number with at least two different digits (leading zeros are allowed).
Excluding 1111 or 0000.

Output format: In the next line we get the space-separated intermediate results until we get Kaprekar's constant

############## q4.sh ############### 

Execution : ./q4.sh

The user is expected to provide a four-digit year that needs to be checked for palindrome days 

Output format: If the palindrome date exists script will print this date if not script will print 
"No Palindrome days available in the given year" message

############## q5.sh ###############  

Execution : ./q5.sh

The user is expected to provide input for which Lucas number Ln needs to be calculated 

Output format: Lucas number of the given input is printed in the next line.