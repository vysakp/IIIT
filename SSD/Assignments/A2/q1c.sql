DROP PROCEDURE IF EXISTS  PTransfer2022;
DELIMITER $$
CREATE PROCEDURE PTransfer2022()
BEGIN
-- declaring the variables for cursor 

    DECLARE emp_id int DEFAULT 0;
	DECLARE f_name varchar(255) DEFAULT "";
	DECLARE l_name varchar(255) DEFAULT "";
    DECLARE cur_reg varchar(255) DEFAULT "";
	DECLARE gen varchar(10) DEFAULT "";
    DECLARE w_exp int DEFAULT 0;
	DECLARE DOJ DATE DEFAULT CURRENT_TIMESTAMP;
    
    

	DECLARE finished int default 0; 
    
-- declaring the cursor 
    DECLARE emp CURSOR FOR SELECT EmpID, FirstName, LastName, Region, Gender, DateOfJoining FROM PERSON;
  --  WHERE `Weight in Kgs.`<50; 
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET finished = 1;


    
-- Drop table if exists
	DROP TABLE IF EXISTS PersonTransfer;
    

--  creating the hike table
	CREATE table PersonTransfer(
		PTID int NOT NULL PRIMARY KEY auto_increment,
		EmpID int, 
		FirstName varchar(255),
		LastName varchar(255),
		Gender varchar(10),
        DateofJoining DATE,
        CurrentRegion varchar(100),
        NewRegion varchar(100),
        FOREIGN KEY (EmpID) REFERENCES PERSON(EmpID)
    );

-- fetching from the person table 		
	
	OPEN emp;
		getemp: LOOP
			FETCH emp INTO emp_id, f_name, l_name, cur_reg, gen, DOJ;
			IF finished = 1 THEN LEAVE getemp;
			END IF;
			-- set values
            SET @w_exp = TIMESTAMPDIFF(YEAR, DOJ, CURRENT_TIMESTAMP);
            IF gen = 'F' AND   @w_exp > 10 THEN
				SET @new_reg = 'DC';
		    ELSEIF gen = 'M' AND   @w_exp > 20 THEN
				  SET @new_reg = 'Capitol';
			ELSE 
				SET @new_reg = cur_reg;
		    END IF;
			
            
			INSERT INTO PersonTransfer (EmpID, FirstName, LastName, Gender, DateofJoining, CurrentRegion, NewRegion)
			VALUES (emp_id, f_name, l_name, gen, DOJ, cur_reg, @new_reg);
			
		END LOOP getemp;
	CLOSE emp;
END$$
DELIMITER ;

CALL PTransfer2022(); 
SELECT * FROM PersonTransfer;


