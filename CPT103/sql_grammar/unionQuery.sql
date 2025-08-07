SOURCE E:/SQL_LEARNING/createExampleTable.sql;

USE runood;

-- USE CONDITION:
-- select sentence must have the same number of columns
-- and the datatype in each column must be the same


-- it has the same datatype
-- both var char
-- SELECT first_name FROM employees
-- UNION
-- SELECT last_name FROM employees
-- ORDER BY first_name;

-- both int
-- the order by is order by the datatype of the chosen column
-- instead of the single column in a table
-- SELECT salary FROM employees
-- UNION 
-- SELECT money FROM orders
-- ORDER BY salary;

-- SHOW COLUMNS FROM ORDERS;

-- use the NULL as placeholder
-- SELECT first_name, salary FROM employees
-- UNION 
-- SELECT NULL, money FROM orders
-- order by salary;

-- if we just check without UNION ALL
-- (SELECT first_name, last_name FROM employees LIMIT 5) 
-- UNION
-- (SELECT first_name, last_name FROM employees LIMIT 5);
-- because the first five result is duplicated
-- we can use the UNION ALL to deal with this shit

-- (SELECT first_name, last_name FROM employees LIMIT 5) 
-- UNION ALL
-- (SELECT first_name, last_name FROM employees LIMIT 5);



