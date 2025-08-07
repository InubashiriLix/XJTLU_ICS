SOURCE E:/SQL_LEARNING/createExampleTable.sql;
USE runood;

-- there are two john, and they share the same salary
-- we can find out
-- SELECT first_name, last_name, salary
-- FROM employees
-- ORDER BY salary DESC, first_name ASC;
-- this command means if the salary is the same,
-- then we use the fisrt name to order

-- use the COLUMNS number to express the columns
-- SELECT first_name, salary, salary
-- FROM employees
-- ORDER BY salary DESC, 1 ASC;

-- an improper example for 
-- using expressions to sort;
-- SELECT money * id AS WTF 
-- FROM orders
-- ORDER BY WTF DESC;

-- the NULLS FIRST and NULLS LAST VERSION:
-- 8.0.16 !!!!

-- using NULL FIRST and NULLS LAST to deal with 
-- NULL values, 
-- SELECT money, `user_id`
-- FROM orders
-- ORDER BY user_id DESC NULLS FIRST;
-- use the NULL_FIRST means if the columns we use to 
-- has a NULL value, then we put this into first place


