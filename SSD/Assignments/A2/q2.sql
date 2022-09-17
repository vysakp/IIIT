DROP FUNCTION IF EXISTS timezoneconvert;
DELIMITER $$
CREATE FUNCTION timezoneconvert(
    sourcedatetimestamp DATETIME,  
    sourcetimezonecode VARCHAR(100),
    targettimezone VARCHAR(100)
)
RETURNS DATETIME
DETERMINISTIC
BEGIN 
	DECLARE targetdatetimestamp DATETIME DEFAULT CURRENT_TIMESTAMP;
    
    -- Getting the epoch time from the given time stamp
	SET @epoch_time = UNIX_TIMESTAMP(sourcedatetimestamp );
    
    -- Storing the GMT offset of source timezone in soffset after sorting the the table in desc order
    SELECT gmtoffset INTO @soffset from time_zone where timestart<=@epoch_time AND timezonecode=sourcetimezonecode ORDER BY timestart DESC LIMIT 1;
    
    -- Storing the GMT offset of target timezone in soffset after sorting the the table in desc order
    SELECT gmtoffset INTO @toffset from time_zone where timestart<=@epoch_time AND timezonecode=targettimezone ORDER BY timestart DESC LIMIT 1;
    
    -- Calculating the taget timestamp in epoch time
    SET @targetdatetimestamp = @epoch_time - @soffset + @toffset;
    
    -- Returning the time in date time format
    RETURN FROM_UNIXTIME(@targetdatetimestamp);
    
    
END$$
DELIMITER ;

SET @sourcedatetimestamp = "29-07-2022 02:53:00";
SET @sourcetimezonecode = "UTC"; 
SET @targettimezone = "EST"; 


SET @result = timezoneconvert(STR_TO_DATE(@sourcedatetimestamp,'%d-%m-%Y %H:%i:%s.%f'), @sourcetimezonecode, @targettimezone);

-- Selecting the output in correct date format
SELECT DATE_FORMAT(@result, '%d-%m-%Y %H:%i:%s') as Converted_time;
    
    



