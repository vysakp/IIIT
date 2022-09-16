DROP PROCEDURE IF EXISTS  Empborn;

DROP TABLE IF EXISTS EmpBorn;
DELIMITER $$
CREATE PROCEDURE Empborn()
BEGIN
-- declaring the variables for cursor 
	DECLARE eb_id int DEFAULT 0;
    DECLARE cur_reg varchar(255) DEFAULT "";
	DECLARE tob time DEFAULT CURRENT_TIMESTAMP;
	DECLARE finished int default 0; 
    
-- declaring the cursor 
    DECLARE emp CURSOR FOR SELECT Region, TimeOfBirth FROM PERSON;
  --  WHERE `Weight in Kgs.`<50; 
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET finished = 1;


    
-- Drop table if exists
	DROP TABLE IF EXISTS PersonTransfer;
    

--  creating the hike table
	CREATE TABLE EmpBorn (
    EBID INT NOT NULL PRIMARY KEY AUTO_INCREMENT,
    Region VARCHAR(255),
    TOB time,
    Born0_8 BOOL,
    Born8_15 BOOL,
    Born15_23 BOOL
);
    -- setting the values
    

-- fetching from the person table 		
	
	OPEN emp;
		getemp: LOOP
			FETCH emp INTO cur_reg, tob;
			IF finished = 1 THEN LEAVE getemp;
			END IF;
			-- set values
            
            
			IF tob >= CAST('00:00:00' AS TIME ) AND tob <= CAST('8:00:00' AS TIME)   THEN
				SET @born0_8= true;
			ELSE 
				SET @born0_8= NULL;
			END IF;
            
            IF tob >= CAST('8:01:00' AS TIME ) AND tob <= CAST('15:00:00' AS TIME)   THEN
				SET @born8_15= true;
			ELSE 
				SET @born8_15= NULL;
			END IF;
            
            IF tob >= CAST('15:01:00' AS TIME ) AND tob <= CAST('22:59:00' AS TIME)   THEN
				SET @born15_23= true;
			ELSE 
				SET @born15_23= NULL;
			END IF;
			

			INSERT INTO EmpBorn (EBID, Region,TOB, Born0_8, Born8_15, Born15_23)
			VALUES (eb_id, cur_reg, tob, @born0_8, @born8_15, @born15_23);
			
		END LOOP getemp;
	CLOSE emp;
    SELECT REGION AS EmployeeRegion, COUNT(Born0_8) AS `No. Of Employee born between 00:00 hours to 08:00 hours`,
    COUNT(born8_15) AS `No. Of Employee born between 08.01 hours to 15.00 hours`,
    COUNT(born15_23) AS `No. Of Employees born after 15:01 hours until 22:59 hours`
    FROM EmpBorn GROUP BY REGION;
    
END$$
DELIMITER ;

CALL Empborn(); 
-- SELECT * FROM EmpBorn;


