SOURCE E:/SQL_LEARNING/createExampleTable.sql;
USE runood;

-- Structure belike

-- SELECT column1, aggregate_function(column2)
-- FROM table_name
-- WHERE condition
-- GROUP BY column1;

-- sum function is not available for varchar type
-- SELECT SUM(department) 
-- FROM employees
-- WHERE salary > 0
-- GROUP BY department;

-- SELECT SUM(salary)
-- FROM employees
-- WHERE department = "HR"
-- GROUP BY salary;

-- use COUNT TO deal with varchar type
-- SELECT department, COUNT(department)
-- FROM employees
-- WHERE salary > 0
-- GROUP BY department;

-- use with rollup to collate the data
-- that has been collate

-- select coaleces(a, b, c)
-- the logic in COALESCE is 
-- def COALESCE(a, b, c):
	-- if a is not null: 
		-- return a
	-- elif b is not null:
		-- reutrn b
	-- elif c is not null:
		-- return c
	-- else:
		-- return null

-- SELECT COALESCE(user_id, "EMPTY"), money 
-- FROM orders;
