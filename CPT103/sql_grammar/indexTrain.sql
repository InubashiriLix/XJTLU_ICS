SOURCE E:/SQL_LEARNING/indexExampleTable.sql;

USE runood;

DROP TABLE IF EXISTS testindex;

-- create a index
-- CREATE INDEX idx_name
-- ON employees (name DESC);

-- add the index to the table
ALTER TABLE employees
ADD INDEX idx_name (name ASC); 

-- create the index with the table
CREATE TABLE IF NOT EXISTS testindex (
	id INT AUTO_INCREMENT PRIMARY KEY,
	name VARCHAR(255) NOT NULL,
	age SMALLINT NOT NULL,
	department VARCHAR(255) NOT NULL,
	hire_date DATE,
	salary DECIMAL(10,2),
	INDEX idx_salary(salary),
	INDEX idx_department(department)
) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci ENGINE=InnoDB;

DROP TABLE IF EXISTS students;

CREATE TABLE IF NOT EXISTS students(
	id INT AUTO_INCREMENT PRIMARY KEY,
	name VARCHAR(50) NOT NULL,
	age INT NOT NULL,
	INDEX idx_age(age),
	INDEX idx_name(name ASC)
);

-- DROP the index
-- DROP INDEX idx_name ON employees;

-- DROP the index from the ALTER
-- ALTER TABLE students
-- DROP INDEX idx_age;

-- make sure that the table and index has been existing
-- SET AUTOCOMMIT = 0;

-- DROP PROCEDURE IF EXISTS process_drop_idx_name_from_student;

-- DELIMITER $$

-- CREATE PROCEDURE process_drop_idx_name_from_student()
-- BEGIN 
	-- DECLARE idx_exists INT DEFAULT 0;

	-- SELECT COUNT(*) INTO idx_exists
	-- FROM information_schema.statistics
	-- WHERE table_schema = DATABASE();
		-- AND table_name = 'student'
		-- AND index_name = 'idx_name';
	
	-- IF idx_exists > 0 THEN 
		-- START TRANSACTION;

		-- DROP INDEX idx_name ON students;

		-- COMMIT;
	-- ELSE
		-- SELECT "NOT EXISTS!!!";
	-- END IF;

-- END$$
-- DELIMITER ;

-- CALL process_drop_idx_name_from_student();

-- create the unique index
-- the unique index will be set when you setup the table
DROP TABLE IF EXISTS test_index_table;
CREATE TABLE IF NOT EXISTS test_index_table (
	id INT AUTO_INCREMENT PRIMARY KEY,
	first_name VARCHAR(255) NOT NULL,
	last_name VARCHAR(255) NOT NULL,
	uid INT NOT NULL,
	CONSTRAINT index1 UNIQUE (first_name ASC, last_name ASC)
);

-- create the unique index individually
-- CREATE UNIQUE INDEX index2 ON test_index_table (first_name ASC);
-- CREATE UNIQUE INDEX index3 ON test_index_table (last_name ASC);
-- CREATE UNIQUE INDEX index4 ON test_index_table (last_name DESC, first_name DESC);

-- use the alter table to create unique index
ALTER table test_index_table
ADD CONSTRAINT unique_constraint_name UNIQUE (first_name DESC, last_name DESC);

-- use the alter table to delete the index
ALTER TABLE test_index_table DROP COLUMN id;
ALTER TABLE test_index_table ADD PRIMARY KEY (uid);


ALTER TABLE test_index_table ADD INDEX index_fn (first_name ASC);
ALTER TABLE test_index_table ADD UNIQUE INDEX index_ln (last_name ASC);
ALTER TABLE test_index_table DROP INDEX index_ln;
-- ALTER TABLE test_index_table ADD FULLTEXT index_nn (last_name ASC);

-- SHOW THE INDEX:
SHOW INDEX FROM test_index_table;
SELECT "";
SHOW INDEX FROM test_index_table\G;
