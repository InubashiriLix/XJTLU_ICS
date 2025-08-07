-- show all the databases
-- SHOW DATABASES \G;

-- select the database
USE runood;

-- check all the table in the database
-- SHOW TABLES;

-- check the structure of the table
-- DESC employees; 
-- SHOW COLUMNS FROM employees;

-- check the index of the table
-- SHOW INDEX FROM employees \G;

-- show teh create command for the table
-- SHOW CREATE TABLE employees;

-- check the columns count of a table
-- SELECT COUNT(*) FROM employees;

-- -- check the infomation of the column 
-- SELECTI
	-- TABLE_NAME, 
	-- COLUMN_NAME,
	-- CONSTRAINT_NAME,
	-- REFERENCED_TABLE_NAME,
	-- REFERENCED_COLUMN_NAME
-- FROM 
	-- INFORMATION_SCHEMA
-- WHERE 
	-- TABLE_SCHEMA = 'runood',
	-- AND TABLE_NAME = 'employees'
	-- AND REFERENCE_TABLE_NAME IS NOT NULL;

		
-- the information_schema database contain all the info about the database 
-- and these information are contined in the information_schema

-- the schemata table contains
-- SELECT * FROM information_schema.SCHEMATA;

-- the columns table contains the column info in the table 
-- SELECT * 
-- FROM information_schema.COLUMNS
-- WHERE TABLE_SCHEMA = 'runood' 
-- AND TABLE_NAME = 'employees';

-- the statistic table
-- this table contains the info about the unique, index name and column name
SELECT * 
FROM information_schema.STATISTICS 
WHERE TABLE_SCHEMA = 'runood'
AND TABLE_NAME = 'employees';

-- the key_column_usage table
-- this contains the infoamtion like foreign key name, 
-- column name, relavant table and ...
SELECT *
FROM information_schema.KEY_COLUMN_USAGE 
WHERE TABLE_SCHEMA = 'runood'
AND TABLE_NAME = 'employees';

-- the referential_constraints table
SELECT * FROM information_schema.REFERENTIAL_CONSTRAINTS
WHERE CONSTRAINT_SCHEMA = 'runood'
AND TABLE_NAME = 'employees';

