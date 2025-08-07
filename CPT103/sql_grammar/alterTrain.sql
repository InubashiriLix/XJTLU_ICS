USE runood;

-- force to delete;
SET FOREIGN_KEY_CHECKS = 0;

-- DROP TABLE IF EXISTS touhou_staff;
DROP TABLE IF EXISTS touhou_salary;
DROP TABLE IF EXISTS touhou;

-- 创建一个新的 Touhou 角色表
CREATE TABLE IF NOT EXISTS touhou(
	id INT AUTO_INCREMENT PRIMARY KEY,
	name VARCHAR(255) NOT NULL UNIQUE,
	
	ability VARCHAR(255),
	affiliation VARCHAR(255)
) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci ENGINE = InnoDB;

SHOW COLUMNS FROM touhou;	



CREATE TABLE IF NOT EXISTS touhou_salary(
	id INT AUTO_INCREMENT PRIMARY KEY,
	touhou_name VARCHAR(255) NOT NULL,
	salary DECIMAL(10, 2) NOT NULL,

	FOREIGN KEY (touhou_name) REFERENCES touhou(name) ON DELETE CASCADE 
) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci ENGINE = InnoDB;
SHOW COLUMNS FROM touhou_salary;


-- add new columns -- ADD
-- ALTER TABLE touhou
-- ADD COLUMN species VARCHAR(255) NOT NULL;
-- SELECT "DONE";
-- SHOW COLUMNS FROM touhou;

-- change the datatype of the column -- MODIFY
-- ALTER TABLE touhou
-- MODIFY COLUMN name CHAR(200) NOT NULL UNIQUE;
-- SELECT "DONE";
-- SHOW COLUMNS FROM touhou;

-- change the columns name -- CHANGE
-- ALTER TABLE touhou
-- CHANGE COLUMN name fullname VARCHAR(200) NOT NULL DEFAULT "invalid" UNIQUE;
-- SELECT "DONE";
-- SHOW COLUMNS FROM touhou;

-- delete A column -- DROP
-- ALTER TABLE touhou
-- DROP COLUMN affiliation;
-- SELECT "DONE";
-- SHOW COLUMNS FROM touhou;

-- change into the primary key
-- ALTER TABLE touhou
-- DROP COLUMN id;
-- ALTER TABLE touhou
-- ADD PRIMARY KEY(name);
-- SELECT "DONE";
-- SHOW COLUMNS FROM touhou;

-- add the foreign key
-- CREATE TABLE IF NOT EXISTS touhou_staff (
	-- id INT AUTO_INCREMENT PRIMARY KEY,
	-- realname VARCHAR(255) NOT NULL UNIQUE,
	-- name VARCHAR(255) NOT NULL UNIQUE,
	-- salary_id INT NOT NULL
-- ) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;

-- SELECT "DONE";

-- ALTER TABLE touhou_staff
-- ADD CONSTRAINT fk_name
-- FOREIGN KEY (name)
-- REFERENCES touhou(name);

-- ALTER TABLE touhou_staff
-- ADD CONSTRAINT fk_salary
-- FOREIGN KEY (salary_id)
-- REFERENCES touhou_salary(id);


-- -- edit the name of table
-- -- drop constraint
-- -- because we still have constraint there that we can not just rename the touhou_staff
-- -- we need to delete the constraints and add them back again
-- ALTER TABLE touhou_staff DROP FOREIGN KEY fk_name;
-- ALTER TABLE touhou_staff DROP FOREIGN KEY fk_salary;

-- ALTER TABLE touhou_staff RENAME TO coser_info;

-- SELECT "DONE";

-- ALTER TABLE coser_info
-- ADD CONSTRAINT fk_name
-- FOREIGN KEY (name)
-- REFERENCES touhou(name);

-- ALTER TABLE coser_info
-- ADD CONSTRAINT fk_salary
-- FOREIGN KEY (salary_id)
-- REFERENCES touhou_salary(id);

-- SHOW COLUMNS FROM coser_info;



-- the NULL will be set if you do not provide a default value
-- add character
-- INSERT INTO touhou (name, ability, affiliation) VALUES
	-- ('Reimu Hakurei', 'Flight, sealing power', 'Hakurei Shrine'),
	-- ('Marisa Kirisame', 'Magic, light-based lasers', 'Kirisame Magic Shop'),
	-- ('Sakuya Izayoi', 'Time manipulation', 'Scarlet Devil Mansion');
-- SELECT * FROM touhou;

-- -- modify the name
-- ALTER TABLE touhou
-- MODIFY COLUMN affiliation SET NULL DEFAULT "invalid";

-- SELECT * FROM touhou;
