DROP PROCEDURE IF EXISTS  set_hike2022;
DELIMITER $$
CREATE PROCEDURE set_hike2022()
BEGIN
-- declaring the variables for cursor 
	DECLARE f_name varchar(255) DEFAULT "";
	DECLARE l_name varchar(255) DEFAULT "";
	DECLARE gen varchar(10) DEFAULT "";
	DECLARE weight_kg int DEFAULT 0;
	DECLARE l_hike varchar(10) DEFAULT "";
	DECLARE l_sal int DEFAULT 0;
    DECLARE emp_id int DEFAULT 0;
    
    DECLARE n_hike varchar(10) DEFAULT "";
    DECLARE n_sal int DEFAULT 0;
    DECLARE pk_hike int DEFAULT 0;
	DECLARE finished int; 
    
-- declaring the cursor 
    DECLARE emp CURSOR FOR SELECT `EmpID`,`FirstName`, `LastName`, `Gender`, `WeightInKgs`, `LastHike`, `Salary` FROM PERSON
    WHERE WeightInKgs < 50;
  --  WHERE `Weight in Kgs.`<50; 
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET finished = 1;


    
-- Drop table if exists
	DROP TABLE IF EXISTS hike2022;
    
-- setting initial value of pk as 0
    SET pk_hike = 0;
    
--  creating the hike table
	CREATE table hike2022(
		HikeID int NOT NULL PRIMARY KEY auto_increment,
		EmpID int, 
		FirstName varchar(255),
		LastName varchar(255),
        Gender varchar(10),
        WeightInKg varchar(255), 
        LastHike varchar(25),
        LastSalary varchar(255),
        NewHike varchar(25),
        NewSalary varchar(255),
        FOREIGN KEY (EmpID) REFERENCES PERSON(`EmpID`)
    );


-- fetching from the person table 		
	
	OPEN emp;
		getemp: LOOP
			FETCH emp INTO emp_id, f_name, l_name, gen, weight_kg, l_hike, l_sal;
			IF finished = 1 THEN LEAVE getemp;
			END IF;
			-- set values
			SET @n_hike = convert(l_hike, unsigned)+2;
            SET @n_sal = ROUND(l_sal*(1+(@n_hike/100)),2);
            SET @n_hike = concat(@n_hike, '%');
			
            
			INSERT INTO hike2022 (EmpID, FirstName, LastName, Gender, WeightInKg, LastHike, LastSalary, NewHike, NewSalary)
			VALUES (emp_id, f_name, l_name, gen, weight_kg, l_hike, l_sal, @n_hike,@n_sal);
			
		END LOOP getemp;
	CLOSE emp;
END$$
DELIMITER ;

CALL set_hike2022(); 
SELECT * FROM hike2022;



