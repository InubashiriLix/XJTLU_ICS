CREATE DATABASE IF NOT EXISTS runood CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE runood;

DROP TABLE IF EXISTS employees1;
DROP TABLE IF EXISTS employees2;
DROP TABLE IF EXISTS employees3;

CREATE TABLE IF NOT EXISTS employees1 (
    id INT AUTO_INCREMENT PRIMARY KEY,          -- 员工唯一标识
	first_name VARCHAR(100) NOT NULL,           -- 员工的名字
	last_name VARCHAR(100) NOT NULL,            -- 员工的姓氏
    department VARCHAR(100) NOT NULL,           -- 员工所属部门
	position VARCHAR(100) NOT NULL,             -- 员工的职位
	salary DECIMAL(10, 2) NOT NULL,             -- 员工的薪水
	hire_date DATE NOT NULL,                    -- 员工的入职日期
	active BOOLEAN NOT NULL DEFAULT TRUE        -- 员工是否在职
) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

CREATE TABLE IF NOT EXISTS employees2 (
    id INT AUTO_INCREMENT PRIMARY KEY,          -- 员工唯一标识
	first_name VARCHAR(100) NOT NULL,           -- 员工的名字
	last_name VARCHAR(100) NOT NULL,            -- 员工的姓氏
    department VARCHAR(100) NOT NULL,           -- 员工所属部门
	position VARCHAR(100) NOT NULL,             -- 员工的职位
	salary DECIMAL(10, 2) NOT NULL,             -- 员工的薪水
	hire_date DATE NOT NULL,                    -- 员工的入职日期
	active BOOLEAN NOT NULL DEFAULT TRUE        -- 员工是否在职
) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

CREATE TABLE IF NOT EXISTS employees3 (
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

INSERT INTO employees1 (first_name, last_name, department, position, salary, hire_date, active)
VALUES
('John', 'Doe', 'Engineering', 'Software Engineer', 70000.00, '2020-05-15', TRUE),
('Jane', 'Smith', 'Engineering', 'DevOps Engineer', 75000.00, '2018-08-10', TRUE),
('Emily', 'Jones', 'Sales', 'Sales Manager', 65000.00, '2017-01-12', TRUE),
('Patricia', 'Anderson', 'Finance', 'CFO', 130000.00, '2014-05-02', TRUE),
('Barbara', 'Thomas', 'Marketing', 'Marketing Specialist', 60000.00,'2020-11-20',FALSE),
('Daniel', 'Moore', 'Marketing', 'Marketing Manager', 70000.00, '2018-12-05', TRUE),
('Jennifer', 'Clark', 'Engineering', 'Data Scientist', 90000.00, '2021-01-14', FALSE),
('Charles', 'Lewis', 'Engineering', 'Software Engineer', 72000.00, '2020-07-01', TRUE),
('Susan', 'Walker', 'Sales', 'Sales Associate', 48000.00, '2019-02-15', TRUE),
('Christopher', 'Hall', 'HR', 'HR Assistant', 40000.00, '2020-06-05', FALSE),
('John1', 'Doe', 'Engineering', 'Software Engineer', 70000.00, '2020-05-15', TRUE),
('John2', 'Doe', 'Engineering', 'Software Engineer', 70000.00, '2020-05-15', TRUE);

INSERT INTO employees2 (first_name, last_name, department, position, salary, hire_date, active)
VALUES
('John', 'Doe', 'Engineering', 'Software Engineer', 70000.00, '2020-05-15', TRUE),
('Jane', 'Smith', 'Engineering', 'DevOps Engineer', 75000.00, '2018-08-10', TRUE),
('Emily', 'Jones', 'Sales', 'Sales Manager', 65000.00, '2017-01-12', TRUE),
('Michael', 'Brown', 'Sales', 'Sales Associate', 50000.00, '2019-03-22', TRUE),
('Sarah', 'Davis', 'HR', 'HR Manager', 80000.00, '2016-09-30', TRUE),
('David', 'Wilson', 'HR', 'Recruiter', 60000.00, '2021-07-11', FALSE),
('James', 'Taylor', 'Engineering', 'CTO', 120000.00, '2015-02-25', TRUE),
('Robert', 'Miller', 'Finance', 'Accountant', 55000.00, '2019-11-17', TRUE);

INSERT INTO employees3 (first_name, last_name, department, position, salary, hire_date, active)
VALUES 
('Emily', 'Jones', 'Sales', 'Sales Manager', 65000.00, '2017-01-12', TRUE);
