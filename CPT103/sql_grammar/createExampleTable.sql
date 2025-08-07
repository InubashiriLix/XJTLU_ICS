CREATE DATABASE IF NOT EXISTS runood CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
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
('Christopher', 'Hall', 'HR', 'HR Assistant', 40000.00, '2020-06-05', FALSE),
('John1', 'Doe', 'Engineering', 'Software Engineer', 70000.00, '2020-05-15', TRUE),
('John2', 'Doe', 'Engineering', 'Software Engineer', 70000.00, '2020-05-15', TRUE);

-- create the other datas
-- erase the table
DROP TABLE IF EXISTS orders;

-- create table
CREATE TABLE IF NOT EXISTS orders(
	`id` INT AUTO_INCREMENT PRIMARY KEY,
	`create_time` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
	`money` DEC(13, 2) NOT NULL,
	-- lets say the user_id can be NULL
	`user_id` INT  
) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

INSERT INTO orders
VALUES 
(DEFAULT, DEFAULT, 100.40, 1),
(DEFAULT, DEFAULT, 200.00, 2),
(DEFAULT, DEFAULT, 3400.0, 4),
(DEFAULT, DEFAULT, 4000.0, 5),
(DEFAULT, DEFAULT, 4000.0, NULL);
