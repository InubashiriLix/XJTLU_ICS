CREATE DATABASE IF NOT EXISTS runood CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

USE runood;

DROP TABLE IF EXISTS text_samples;

-- 创建表格存储多种文本类型，使用 utf8mb4 字符集以支持中文和其他特殊字符
CREATE TABLE IF NOT EXISTS text_samples (
    id INT AUTO_INCREMENT PRIMARY KEY,
    description VARCHAR(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci,   -- 描述文本类型的字段
    content TEXT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL       -- 存储具体文本内容的字段
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 插入提供的文本数据
-- Drop the table if it exists
DROP TABLE IF EXISTS text_samples;

-- Create a table to store multiple types of text data, using utf8mb4 charset to support all characters
CREATE TABLE IF NOT EXISTS text_samples (
	id INT AUTO_INCREMENT PRIMARY KEY,
	description VARCHAR(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,   -- Description field
	content TEXT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL                -- Content field for the actual text
	) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

	-- Insert sample data into the table
	INSERT INTO text_samples (description, content) VALUES
	('HTML snippet', '<html><head><title>My Website</title></head><body><h1>Hello, World!</h1></body></html>'),
	('Data report with name, age, and email', 'Name: John Doe, Age: 30, Email: john@example.com'),
	('Common paragraph', 'Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Maecenas porttitor congue massa. Fusce posuere, magna sed pulvinar ultricies, purus lectus malesuada libero, sit amet commodo magna eros quis urna.
		Nunc viverra imperdiet enim. Fusce est. Vivamus a tellus.
		Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Proin pharetra nonummy pede. Mauris et orci.
		Aenean nec lorem. In porttitor. Donec laoreet nonummy augue.
		Suspendisse dui purus, scelerisque at, vulputate vitae, pretium mattis, nunc. Mauris eget neque at sem venenatis eleifend. Ut nonummy.
		'),
	('Linux system path', '/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin'),
	('URL example', 'https://www.example.com/index.html?user=test&lang=en'),
	('Log entry with IP address and timestamp', '192.168.1.1 - - [10/Oct/2023:13:55:36 -0700] "GET /index.html HTTP/1.1" 200 2326'),
	('Short message with order number', 'Your order number is 123456. Please keep this number for future reference.'),
	('Short message with order number', 'Your order number is 123. Please keep this number for future reference.'),
	('Data report with date and temperature', 'Date: 2023-10-15, Time: 14:30:00, Temperature: 22.5C'),
	('SQL query', "SELECT * FROM employees WHERE age > 25 AND department = \'Sales\';");
	
				

