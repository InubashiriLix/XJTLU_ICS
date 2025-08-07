USE runood;

DROP TABLE IF EXISTS null_test;

CREATE TABLE IF NOT EXISTS null_test (
    id INT AUTO_INCREMENT PRIMARY KEY,   -- 自动递增的主键
    first_name VARCHAR(50),              -- 姓
    last_name VARCHAR(50) NOT NULL,      -- 名，不能为空
    age INT,                             -- 年龄，允许为 NULL
    salary FLOAT,                        -- 工资，允许为 NULL
    hire_date DATE,                      -- 入职日期，允许为 NULL
    is_active BOOLEAN,                   -- 是否激活，允许为 NULL
    department_id INT,                   -- 部门ID，允许为 NULL
    comments TEXT                        -- 备注信息，允许为 NULL
);

-- 插入包含 NULL 值的数据
INSERT INTO null_test (first_name, last_name, age, salary, hire_date, is_active, department_id, comments)
VALUES 
    ('John', 'Doe', NULL, 70000, '2020-05-15', TRUE, 1, 'Senior Developer'),
	-- first_name 为 NULL
    (NULL, 'Smith', 30, 75000, '2018-08-10', TRUE, 2, 'Team Lead'),
	 -- salary 和 department_id 为 NULL
    ('Emily', 'Jones', 35, NULL, '2017-01-12', TRUE, NULL, 'Sales Manager'),
    -- age 和 comments 为 NULL	
    ('Patricia', 'Anderson', NULL, 130000, '2014-05-02', TRUE, 3, NULL), 
	-- is_active 为 NULL
    ('Barbara', 'Thomas', 29, 60000, '2020-11-20', NULL, 4, 'Marketing Specialist'),
	-- 全部字段（除了 last_name）都为 NULL
    ('Daniel', 'Moore', NULL, NULL, NULL, NULL, NULL, NULL); 


-- 插入更多数据来演示 NULL 比较
INSERT INTO null_test (first_name, last_name, age, salary, hire_date, is_active, department_id, comments)
VALUES
    ('Charles', 'Lewis', 42, 72000, NULL, FALSE, 1, NULL),
    ('Susan', 'Walker', NULL, NULL, NULL, TRUE, NULL, 'Temporary employee'),
	(NULL, "Inubashiri", NULL, NULL, NULL, TRUE, NULL, "fuck you");

