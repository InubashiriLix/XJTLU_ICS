-- if the database does not exists,
-- then create the database
CREATE DATABASE IF NOT EXISTS runood CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
-- use the database
USE runood;

-- create two tables
CREATE TABLE IF NOT EXISTS `user` (
	`id` INT AUTO_INCREMENT PRIMARY KEY,
	`username` VARCHAR(100) NOT NULL,
	`email` VARCHAR(100) NOT NULL,
	`birthday` DATE,
	`is_active` BOOLEAN DEFAULT TRUE,
	CONSTRAINT uc_user_email UNIQUE (`email`)
) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

CREATE TABLE IF NOT EXISTS `runood_tbl`(
	`runood_id` INT AUTO_INCREMENT PRIMARY KEY,
	`runood_title` VARCHAR(200) NOT NULL,
	`runood_author_name` VARCHAR(100) NOT NULL,
	`submission_date` DATE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

SELECT "";
SHOW COLUMNS FROM runood.user;
SELECT "";
SHOW COLUMNS FROM runood.runood_tbl;


DROP TABLE IF EXISTS user;
TRUNCATE TABLE runood_tbl;

-- it should do not have any output now
ELECT "";
SHOW COLUMNS FROM runood.user;
SELECT "";
SHOW COLUMNS FROM runood.runood_tbl;
