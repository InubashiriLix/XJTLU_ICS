USE runood;

CREATE TEMPORARY TABLE last_night 
(
	ID INT AUTO_INCREMENT PRIMARY KEY,
	name VARCHAR(255) NOT NULL UNIQUE
);

INSERT INTO last_night VALUES
(DEFAULT, "Inubashiri");

SHOW CREATE TABLE last_night \G;

-- copy the data in the output --> 
CREATE TEMPORARY TABLE `last_night_cp` (
  `ID` int NOT NULL AUTO_INCREMENT,
  `name` varchar(255) COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- insert original data: no VALUES!!!!
INSERT INTO last_night_cp (ID, name)  
SELECT ID, name FROM last_night;

SELECT * FROM last_night_cp;

-- we can also use teh mysqldump command
-- mysqldump -u username -p dbname old_table > old_table_dump.sql
