-- create a new database there
DROP DATABASE IF EXISTS `stirb1`;
DROP DATABASE IF EXISTS `stirb2`;


CREATE DATABASE IF NOT EXISTS `stirb1`
CHARACTER SET utf8mb4
COLLATE utf8mb4_general_ci;

CREATE DATABASE IF NOT EXISTS `stirb2`
CHARACTER SET utf8mb4
COLLATE utf8mb4_general_ci;

-- see all the databases
SELECT ""; -- next line
SELECT ("-- all the database there");
SHOW DATABASES;

-- use the database
USE `stirb1`;

-- create new tables
CREATE TABLE IF NOT EXISTS `table1`(
	`name` VARCHAR(100),
	`telnumber` INT(11)
	);

CREATE TABLE IF NOT EXISTS `table2`(
	`name` VARCHAR(100),
	`telnumber` INT(100),
	`postAddress` CHAR(8));

-- use the table `stirb2`
USE `STIRB2`;

-- create two tables
SELECT "create the database Stirb2";

CREATE TABLE IF NOT EXISTS `table1`(
	`name` VARCHAR(100),
	`money` DEC(10, 2)
);

CREATE TABLE IF NOT EXISTS `table2`(
	`uid` INT(11) NOT NULL PRIMARY KEY,
	`id` INT(10) NOT NULL,
	`name` VARCHAR(100) NOT NULL,
	`state` SMALLINT NOT NULL DEFAULT 1,
	`post` SMALLINT(8),
	`money` DEC(12, 2)	
);

-- show the list info in the table2 in the stirb2
SELECT ""; -- next line
SELECT "-- check the list infos in the table2 in the stirb2";
USE `stirb2`;
SHOW COLUMNS FROM TABLE2;

USE `stirb1`;
SHOW COLUMNS FROM TABLE1;

