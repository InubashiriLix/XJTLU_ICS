-- the temporary table means that 
-- the table will be destroied when you 
-- are leaving the client connection
use runood;

-- 修改 DELIMITER 为 $$
DELIMITER $$

-- 创建临时表
CREATE TEMPORARY TABLE temp_table_name(
	column1 INT AUTO_INCREMENT PRIMARY KEY,
	name VARCHAR(255) NOT NULL
) $$

-- 恢复 DELIMITER
DELIMITER ;

INSERT INTO temp_table_name VALUES 
(DEFAULT, "Inubashiri");

SELECT * FROM temp_table_name;

ALTER TABLE temp_table_name
ADD COLUMN uid INT NOT NULL;
-- 显示表结构
-- DESCRIBE temp_table_name;


