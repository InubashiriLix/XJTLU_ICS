SOURCE E:/SQL_LEARNING/createExampleTable.sql

use runood;


-- SELECT ABS(-1);
-- 1

-- SELECT ACOS(0.25);

-- SELECT ASIN(0.5);

-- SELECT ATAN(0.5);

-- SELECT ATAN2(-0.8, 2);

-- SHOW COLUMNS FROM employees;
-- SELECT AVG(COALESCE(salary, 0)) AS avg_salary
-- FROM employees;

-- the ceil will return the bigest int that is just bigger than 
-- the value inside
-- SELECT CEIL(salary) AS ceil_salary
-- FROM employees;
-- SELECT CEILING(salary) AS ceil_salary
-- FROM employees;

-- cos(x)
-- SELECT COS(3.14 * 90);

-- COT(x)
-- SELECT COT(6);

-- COUNT
-- SELECT COUNT(*) as department_count
-- FROM employees
-- WHERE department IS NOT NULL
-- GROUP BY department;

-- DEGREE(X)

SELECT salary DIV 1000 AS divided
FROM employees;

SELECT MOD(salary, 10) FROM employees;


-- EXP(x)
-- SELECT EXP(2);

-- SELECT LN(2);

-- SELECT LOG(e);
-- SELECT LOG(e, 8);

-- SELECT LOG10(100);
-- SELECT LOG2(8);

-- FLOOR(x)
-- SELECT FLOOR(salary) as floored 
-- FROM employees
-- WHERE salary IS NOT NULL;

SELECT MAX(salary) AS highest_salary
FROM employees;

SELECT GREATEST(1, 2, 3);

SELECT LEAST(1, 2, 3, 4);

SELECT PI();
SELECT PI() * MAX(salary) AS salaries
FROM employees;

SELECT POW(10, 2);
SELECT POWER(10, 2); 

SELECT CEIL(RAND() * 100);

SELECT ROUND(RAND() * 10);

SELECT SIGN(-10);

SELECT SQRT(4);

SELECT SUM(salary) FROM employees;
SELECT COUNT(salary) FROM employees;

SELECT TRUNCATE(10.113586, 4);
-- 10.1135
SELECT ROUND(10.113586, 4);
-- 10.1136


