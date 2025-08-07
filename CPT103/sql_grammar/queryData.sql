-- set the time_zone 
SET time_zone = "+08:00";

-- create a better table for query
-- a employee sheet should be better
USE runood;

DROP TABLE IF EXISTS employees;

CREATE TABLE IF NOT EXISTS employees (
    id INT AUTO_INCREMENT PRIMARY KEY,          -- 员工唯一标识
	first_name VARCHAR(100) NOT NULL,           -- 员工的名字
	last_name VARCHAR(100) NOT NULL,            -- 员工的姓氏
    department VARCHAR(100) NOT NULL,           -- 员工所属部门
	position VARCHAR(100) NOT NULL,             -- 员工的职位
	salary DECIMAL(10, 2) NOT NULL,             -- 员工的薪水
	hire_date DATE NOT NULL,                    -- 员工的入职日期
	active BOOLEAN NOT NULL DEFAULT TRUE        -- 员工是否在职
) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

-- 插入一些数据
INSERT INTO employees (first_name, last_name, department, position, salary, hire_date, active)
VALUES
('John', 'Doe', 'Engineering', 'Software Engineer', 70000.00, '2020-05-15', TRUE),
('Jane', 'Smith', 'Engineering', 'DevOps Engineer', 75000.00, '2018-08-10', TRUE),
('Emily', 'Jones', 'Sales', 'Sales Manager', 65000.00, '2017-01-12', TRUE),
('Michael', 'Brown', 'Sales', 'Sales Associate', 50000.00, '2019-03-22', TRUE),
('Sarah', 'Davis', 'HR', 'HR Manager', 80000.00, '2016-09-30', TRUE),
('David', 'Wilson', 'HR', 'Recruiter', 60000.00, '2021-07-11', FALSE),
('James', 'Taylor', 'Engineering', 'CTO', 120000.00, '2015-02-25', TRUE),
('Robert', 'Miller', 'Finance', 'Accountant', 55000.00, '2019-11-17', TRUE),
('Patricia', 'Anderson', 'Finance', 'CFO', 130000.00, '2014-05-02', TRUE),
('Barbara', 'Thomas', 'Marketing', 'Marketing Specialist', 60000.00,'2020-11-20',FALSE),
('Daniel', 'Moore', 'Marketing', 'Marketing Manager', 70000.00, '2018-12-05', TRUE),
('Jennifer', 'Clark', 'Engineering', 'Data Scientist', 90000.00, '2021-01-14', FALSE),
('Charles', 'Lewis', 'Engineering', 'Software Engineer', 72000.00, '2020-07-01', TRUE),
('Susan', 'Walker', 'Sales', 'Sales Associate', 48000.00, '2019-02-15', TRUE),
('Christopher', 'Hall', 'HR', 'HR Assistant', 40000.00, '2020-06-05', FALSE);

-- have a view of the table
-- SELECT * FROM employees;

-- SELECT column1, column2, ...
-- FROM table_name
-- [WHERE condition]
-- [ORDER BY column_name [ASC | DESC]]
-- [LIMIT number];

-- select all the data from the table
-- SELECT * FROM employees;

-- find all the people in the Engineering department:
-- SELECT * FROM employees 
-- WHERE department = "Engineering"
-- ORDER BY hire_date ASC;

-- find employees who has salary that higher than 70000
-- SELECT first_name, last_name, salary, department, active
-- FROM employeesD
-- WHERE salary > 70000
-- ORDER BY salary DESC
-- LIMIT 10;

-- calculate the average of salary
-- SELECT AVG(salary) AS average_salary FROM employees;

-- group by the department, calculate the number of each department
SELECT department, COUNT(*) AS number_of_department
FROM employees
GROUP BY department;

-- find all the employees who has salary that is higher than 70000
-- SELECT first_name, last_name, salary 
-- FROM employees 
-- WHERE salary > 7000
-- ORDER BY salary DESC
-- LIMIT 5;

-- use the Like and the and to finda data
-- SELECT first_name, last_name, salary
-- FROM employees
-- WHERE first_name LIKE "j%" AND active = TRUE
-- ORDER BY salary DESC
-- limit 4;

-- use the like and or to find someone
-- SELECT first_name, last_name, salary
-- FROM employees
-- WHERE first_name LIKE "j%" OR salary > 8000
-- ORDER BY salary;
-- SELECT "";

-- use multi conditions
-- SELECT first_name, last_name, salary, active
-- FROM employees
-- WHERE (first_name LIKE "J%" OR salary > 8000) AND active
-- ORDER BY salary ASC;
-- SELECT "";

-- use in conditions
-- SELECT first_name, last_name, salary
-- FROM employees
-- WHERE first_name IN ("John", "Daniel") AND active = TRUE;

-- WHERE conditions
-- we got <, >, !=, =, <>(!=), <=, >=;
-- you can use complex AND and OR conditions there
-- can also use DELETE and UPDATE command
-- NOT, IS NULL, IS NOT NULL, BETWEEN conditions can also be there

-- NOT + IS NOT NULL 
-- (we don't have NULL data in the form so that we can only use IS NOT NULL)
-- SELECT first_name, last_name, salary, active
-- FROM employees
-- WHERE first_name IS NOT NULL AND NOT active;

-- SELECT "";

-- -- USE BETWEEN CONDITION
-- -- the conditions in between should use AND !!!
-- SELECT first_name, last_name, salary, hire_date, active
-- FROM employees
-- WHERE active AND hire_date BETWEEN 2018-01-01 AND 2020-12-30;

-- !!! the string in where is not captal sensitive, 
-- you should use BINARY key word to tell them
-- SELECT * 
-- FROM employees 
-- WHERE BINARY first_name = "John";

