SOURCE E:/SQL_LEARNING/createExampleTable.sql;

USE runood;

-- SELECT * FROM employees;

-- SELECT * FROM orders;

-- like sentence is quite like the equal
-- or we can say that the like sentence without _ and % is =
SELECT first_name, last_name, salary 
FROM employees
WHERE last_name LIKE "Doe";
SELECT "";

-- use the symbol % to express any character
SELECT first_name, last_name, salary 
FROM employees
WHERE last_name LIKE "D%";
SELECT "";

-- the '_' means compatiate any character 
SELECT first_name, last_name, salary
FROM employees
WHERE last_name like "_o%";
SELECT "";

-- snsert a new user
INSERT INTO employees (first_name, last_name, department, position, salary, hire_date, active) 
VALUES
("Momiji", "Inubashiri", "CEO", "CEO", 10000, "2022-12-12", TRUE);

-- like without telling capital char
SELECT first_name, last_name, salary
FROM employees
WHERE last_name LIKE "Inub%";
