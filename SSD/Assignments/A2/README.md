Question 1 

    a) Here we are using a cursor emp for going through the PERSON table and finding the employee which is
       having weight < 50.
       If the hike2022 table already exists we are dropping it and creating a new one. 
       Now we are looping with the cursor and populating the rows of the new hike2022 table based on the criterion mentioned in the question.
    
    b) In PJoining2022 stored procedure we are declaring a cursor variable over the PERSON table.
       Now we are dropping the PersonJoining table if it exists and then creating a new PersonJoining table so that on multiple calls of the same procedure we are recreating the table with updated data.
       We loop through the person table with the help of the declared cursor and calculate Age, DayofJoining, MonthofJoining, YearofJoining, and WorkExpinDays values for adding them to the new PersonJoining table.
    
    c) In PTransfer2022 stored procedure, we are declaring the cursor(emp) variable over the PERSON table.
       Now we are dropping the PersonTransfer table if it exists and then creating a new person table so that on multiple calls of the PTransfer2022 procedure we are creating a new PersonTransfer table with the updated variables.
       Then we loop through the person table with the help of the declared cursor variable and then dynamically add the rows to the PTransfer2022 table based on the criterion given in the question.

Question 2

    We are creating a function named timezoneconvert which takes source date time, source timezone, and target timezone and returns the target date time
    First, we get the GMT offset of all the entries which is having the source timezonecode and is less that that of the source date time. We order this table in descending order and take the GMT offset of the first column as the source offset.
    Now, we calculate the target offset in a similar manner to the target timezone.

    We calculate the target time by subtracting the source offset from the source time and then adding the target offset to it. 
    In the end, the result is modified to get the result in the desired date and time format.


Question 3
    
    We have created a stored procedure under the name Empborn, in this stored procedure we are declaring a cursor(emp) over the PERSON table. 
    Now we declare 3 boolean variables Born0_8, Born8_15, and Born15_23 for storing the people born at a different time as given in the question.
    While looping with the help of the cursor we are setting the boolean variable based on the time of berth of the employee. 
    Finally, we are selecting the table showing the count of emp born in different time groups and grouping the table by REGION.
