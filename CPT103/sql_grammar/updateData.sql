SOURCE E:/SQL_LEARNING/createExampleTable.sql
USE runood;

-- update values in the table
UPDATE employees
SET salary = 100000 
WHERE BINARY first_name = 'John';

-- check if the value has been updated
SELECT first_name, last_name, salary
FROM employees 
WHERE BINARY first_name = 'John';

-- update value for single list:
SELECT "";

UPDATE employees 
SET first_name = "Inubashiri"
WHERE BINARY first_name = 'John';

SELECT first_name, last_name, salary
FROM employees
WHERE BINARY first_name = "Inubashiri";

-- update mutliple values in multiple list
SELECT "";

UPDATE employees
SET first_name = 'Momiji', last_name = 'Inubashiri'
WHERE BINARY first_name = 'Inubashiri';

SELECT last_name, salary, active
FROM employees
WHERE last_name = "Inubashiri";

-- update ones salary by calculating
SELECT "";

SELECT salary 
FROM employees
WHERE BINARY first_name = "Momiji";

UPDATE employees
SET salary = salary + 10000
WHERE BINARY first_name = "Momiji";

SELECT salary 
FROM employees
WHERE BINARY first_name = "Momiji";

-- update value of mutliple columns
SELECT first_name, last_name, salary, active
FROM employees
WHERE first_name = "Momiji" AND last_name = "Inubashiri";

UPDATE employees
SET active = FALSE
WHERE BINARY first_name = "Momiji";

SELECT first_name, last_name, salary, active
FROM employees
WHERE first_name = "Momiji" AND last_name = "Inubashiri";

-- update through using the value of subquery
UPDATE employees 
SET salary = ( 
	SELECT SUM(money)
	FROM orders
	WHERE user_id = 1 OR user_id = 2
) 
WHERE first_name = "momiji";

SELECT first_name, last_name, salary, active
FROM employees
WHERE first_name = "Momiji" AND last_name = "Inubashiri";""""
