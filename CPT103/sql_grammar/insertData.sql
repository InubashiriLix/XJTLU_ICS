-- IF NOT EXISTS 确保数据库不存在时才创建，防止报错
CREATE DATABASE IF NOT EXISTS `RUNOOD` CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

-- 切换到刚刚创建的数据库 runood
USE runood;

-- 设置时区为中国时区，即东八区 (+08:00)
-- 设置时区确保时间相关的字段（如 TIMESTAMP）会根据正确的时区显示时间
SET time_zone = "+08:00"; 
-- 东八区是中国标准时间 (CST)，确保记录的时间是当地时间

-- 删除表 accounts 如果存在
-- 这是为了防止之前的表定义不正确或数据冲突
DROP TABLE IF EXISTS accounts;

-- 创建表 accounts
CREATE TABLE IF NOT EXISTS `accounts`(
    `id` INT AUTO_INCREMENT PRIMARY KEY,
	 -- id 字段为主键，AUTO_INCREMENT 确保每条记录自增
    `username` VARCHAR(100) NOT NULL,
	 -- username 必填字段，最大长度为 100 字符
    `email` VARCHAR(100) NOT NULL UNIQUE,
	-- email 必填且唯一，UNIQUE 防止重复的邮箱
    `money` DEC(15, 2) NOT NULL DEFAULT 0.00, 
	-- money 字段使用 DECIMAL 类型，支持 15 位整数和 2 位小数，默认值为 0.00
    `birthday` DATE NOT NULL, 
	-- birthday 必填字段，存储用户的生日，格式为 YYYY-MM-DD
    `create_time` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, 
	-- 创建时间字段，记录每条记录的插入时间，使用默认的当前时间戳
    `last` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, 
	-- 最近更新时间字段，每次更新时自动更新为当前时间
    `is_active` BOOLEAN NOT NULL DEFAULT TRUE 
	-- 用户是否活跃的状态，默认为 TRUE
) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
-- 使用 utf8mb4 字符集支持更多的字符（包括 emoji），并使用通用的校对规则 utf8mb4_general_ci

-- 查看表结构，确认字段定义是否正确
SHOW COLUMNS FROM accounts;

-- 分隔符用于分隔不同类型的查询输出
SELECT ""; 
SELECT "INSERTING DATAS"; -- 提示正在插入数据

-- 插入第一条记录
-- INSERT INTO accounts 语句中指定了具体的列名，可以让其他列使用默认值
-- 其中，`money` 和 `birthday` 是必须的字段，而 `create_time` 和 `last` 字段会根据默认值自动生成
INSERT INTO accounts (`username`, `email`, `money`, `birthday`)
    VALUES ("Inubashiri", '13329001003@163.com', 100.00, "2004-11-29");

-- 查看插入后的数据
SELECT * FROM accounts;

-- 插入第二条记录，使用 DEFAULT 关键字让某些字段自动生成默认值
-- 这种方式可以省略字段名，但必须保证字段的顺序与表定义完全一致
-- AUTO_INCREMENT 字段使用 DEFAULT 来让 MySQL 自动生成 id
-- `create_time` 和 `last` 使用 DEFAULT 让 MySQL 自动填充当前时间戳
-- `is_active` 使用 DEFAULT 来自动设置为 TRUE
INSERT INTO accounts VALUES (DEFAULT, 'Aya', '123@163.com', 1000.00, '2003-11-29', DEFAULT, DEFAULT, DEFAULT);

-- 查看所有插入的数据，验证数据是否正确
SELECT * FROM accounts;

-- 插入多条数据
INSERT INTO accounts (`username`, `email`, `money`, `birthday`)
	VALUES ('test1', '123@gmail.com', 111.00, '2000-01-01'),
			('test2', '123.mail', 123.00, '2020-01-01');

SELECT * FROM accounts;
