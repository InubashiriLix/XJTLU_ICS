SOURCE E:/SQL_LEARNING/nullExampleTable.sql;

USE runood;

-- find the employees where first_name is NULL
-- SELECT * FROM null_test WHERE first_name IS NULL;

-- use the aggregate to process the null value
-- SELECT AVG(COALESCE(salary, 0)) as average_salary FROM null_test;

-- use the IF NULL to substitude the NULL
-- SELECT last_name, IFNULL(first_name, "UNKOWN") FROM null_test;

-- use the <=> to substitude the IS NULL
SELECT * FROM null_test WHERE first_name <=> NULL;

-- the SUM, COUNT, AVG may ignore the null value

-- SUM 
SELECT SUM(salary) FROM null_test;
-- if we use the coalesce to substitude the null value in the salary column
SELECT SUM(COALESCE(salary, 100)) FROM null_test;

-- COUNT
SELECT department_id, COUNT(department_id) 
FROM null_test
GROUP BY department_id;
-- the null will be ignore to 0

SELECT department_id, COUNT(COALESCE(department_id, 5)) 
FROM null_test
GROUP BY department_id;
-- the null will be counted if we use coalesce

