SOURCE E:/SQL_LEARNING/joinExampleTable.sql;
USE runood;

-- so imaging the two tables as two set 
-- the INNER join equals to useing the union of this set

-- the employees1 and employees2 has three same lines
-- use INNER  JOIN to find the same row without other rows

-- if two tables has the same columns, then we should 
-- specify the name of table
-- SELECT employees1.first_name, employees1.last_name
-- FROM employees1
-- INNER JOIN employees2 ON employees2.first_name = employees1.first_name;
 
-- ATTENTION THAT 
-- the table after INNER JOIN should be not same as the table1
-- (which means that it should be table2)
-- and the table2.attri should in the first place as we use ON_COMPLETION

-- pay attention to the sequence of table and attri

-- use the alias of the table
-- SELECT e1.first_name, e2.last_name
-- FROM employees1 AS e1
-- INNER JOIN employees2 AS e2 ON e1.first_name = e2.first_name;

-- the usage of INNER JOIN in multi tables
-- SELECT e1.first_name, e2.first_name, e3.first_name
-- FROM employees1 as e1
-- INNER JOIN employees2 as e2 ON e1.first_name = e2.first_name
-- INNER JOIN employees3 as e3 ON e3.first_name = e2.first_name;

-- use the where sentence to filter
-- SELECT e1.first_name, e2.first_name, e1.salary, e2.salary
-- FROM employees1 as e1
-- INNER JOIN employees2 as e2 ON e1.first_name = e2.first_name
-- WHERE e1.salary IN (70000.00, 75000.00, 65000.00);

-- use LEFT JOIN
-- it is equal to 
-- e1 U (e1 N e2)
-- SELECT e1.first_name
-- FROM employees1 AS e1
-- LEFT JOIN employees2 AS e2 ON e1.first_name = e2.first_name;

-- LEFT JOIN + multi tables
-- SELECT e1.first_name
-- FROM employees as e1
-- LEFT JOIN employees2 AS e2 ON e1.first_name = e2.first_name
-- LEFT JOIN employees3 as e3 ON e2.first_name = e3.first_name;

-- skip the right join
