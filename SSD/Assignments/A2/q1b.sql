-- Need to fix month and day name
DELIMITER $$
CREATE PROCEDURE PJoining2022()
BEGIN
-- declaring the variables for cursor 

    DECLARE emp_id int DEFAULT 0;
	DECLARE f_name varchar(255) DEFAULT "";
	DECLARE l_name varchar(255) DEFAULT "";
	DECLARE DOB DATE DEFAULT CURRENT_TIMESTAMP;
	DECLARE DOJ DATE DEFAULT CURRENT_TIMESTAMP;
    
    

	DECLARE finished int default 0; 
    
-- declaring the cursor 
    DECLARE emp CURSOR FOR SELECT EmpID, FirstName, LastName, DateOfBirth, DateOfJoining FROM PERSON;
  --  WHERE `Weight in Kgs.`<50; 
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET finished = 1;


    
-- Drop table if exists
	DROP TABLE IF EXISTS PersonJoining;
    

--  creating the hike table
	CREATE table PersonJoining(
		PJoinID int NOT NULL PRIMARY KEY auto_increment,
		EmpID int, 
		FirstName varchar(255),
		LastName varchar(255),
        DateOfBirth DATE,
        Age int, 
        DateofJoining DATE,
        DayofJoining int,
        MonthofJoining varchar(20),
        YearofJoining int,
        WorkExpinDays int,
        FOREIGN KEY (EmpID) REFERENCES PERSON(EmpID)
    );

-- fetching from the person table 		
	
	OPEN emp;
		getemp: LOOP
			FETCH emp INTO emp_id, f_name, l_name, DOB, DOJ;
			IF finished = 1 THEN LEAVE getemp;
			END IF;
			-- set values
			SET @Age = TIMESTAMPDIFF(YEAR, DOB, CURRENT_TIMESTAMP);
            SET @DayofJoining = EXTRACT(DAY FROM DOJ);
            SET @MonthofJoining = EXTRACT(MONTH FROM DOJ);
            SET @YearofJoining = EXTRACT(YEAR FROM DOJ);
            SET @WorkExpinDays = TIMESTAMPDIFF(DAY, DOJ, CURRENT_TIMESTAMP);
			
            
			INSERT INTO PersonJoining (EmpID, FirstName, LastName, DateOfBirth, Age, DateofJoining, DayOfJoining, MonthofJoining, YearofJoining, WorkExpinDays)
			VALUES (emp_id, f_name, l_name, DOB, @Age, DOJ,@DayofJoining, @MonthofJoining, @YearofJoining,@WorkExpinDays);
			
		END LOOP getemp;
	CLOSE emp;
END$$
DELIMITER ;

CALL PJoining2022(); 
SELECT * FROM PersonJoining;


