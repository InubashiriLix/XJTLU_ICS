USE runood;

-- 删除表（如果存在）
DROP TABLE IF EXISTS employees;

-- 创建员工表
CREATE TABLE employees (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    email VARCHAR(100) NOT NULL UNIQUE,
    age INT NOT NULL,
    department VARCHAR(50),
    hire_date DATE,
    salary DECIMAL(10, 2),
    INDEX idx_department (department),
    INDEX idx_age (age)
) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

-- 创建一个唯一索引以确保电子邮件唯一
ALTER TABLE employees
ADD CONSTRAINT unique_email UNIQUE (email);


-- 插入数据
INSERT INTO employees (name, email, age, department, hire_date, salary)
VALUES 
('Alice', 'alice@example.com', 30, 'HR', '2020-01-15', 60000.00),
('Bob', 'bob@example.com', 25, 'IT', '2019-03-22', 50000.00),
('Charlie', 'charlie@example.com', 28, 'Sales', '2021-07-10', 55000.00),
('David', 'david@example.com', 35, 'IT', '2018-11-30', 70000.00),
('Eve', 'eve@example.com', 40, 'HR', '2017-05-05', 75000.00),
('Frank', 'frank@example.com', 26, 'Finance', '2022-02-01', 52000.00),
('Grace', 'grace@example.com', 32, 'Sales', '2021-04-20', 58000.00),
('Heidi', 'heidi@example.com', 29, 'IT', '2020-06-15', 62000.00),
('Ivan', 'ivan@example.com', 27, 'Finance', '2019-12-01', 53000.00),
('Judy', 'judy@example.com', 31, 'HR', '2021-08-25', 59000.00);

-- 批量插入示例
INSERT INTO employees (name, email, age, department, hire_date, salary)
SELECT
    CONCAT('Employee', seq) AS name,
    CONCAT('employee', seq, '@example.com') AS email,
    FLOOR(20 + RAND() * 30) AS age,
    ELT(FLOOR(1 + RAND() * 4), 'HR', 'IT', 'Sales', 'Finance') AS department,
    DATE_SUB(CURDATE(), INTERVAL FLOOR(RAND() * 365) DAY) AS hire_date,
    ROUND(30000 + RAND() * 70000, 2) AS salary
FROM
    (SELECT @rownum := @rownum + 1 AS seq FROM
    (SELECT 1 UNION ALL SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL SELECT 5) t1,
    (SELECT 1 UNION ALL SELECT 2 UNION ALL SELECT 3 UNION ALL SELECT 4 UNION ALL SELECT 5) t2,
    (SELECT @rownum := 0) r) AS numbers
LIMIT 1000;  -- 插入1000条数据


-- SELECT * FROM employees LIMIT 50;
