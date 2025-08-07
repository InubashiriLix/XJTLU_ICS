-- how to find whether a table or a database is using Innodb
-- SHOW TABLE STATUS FROM runood;
-- SELECT "Done";

-- SHOW TABLE STATUS FROM runood LIKE "employees_";
-- SELECT "Done";

-- SHOW TABLE_NAME, ENGINE
-- FROM information_schema.TABLES
-- WHERE TABLE_SCHEMA = "employees1";
-- SELECT "DONE";

-- SOURCE E:/SQL_LEARNING/createExampleTable.sql

USE runood;

DROP TABLE IF EXISTS touhou;

-- 创建一个新的 Touhou 角色表
CREATE TABLE IF NOT EXISTS touhou(
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255) NOT NULL UNIQUE,
    species VARCHAR(255) NOT NULL,
    ability VARCHAR(255),
    affiliation VARCHAR(255)
) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci ENGINE = InnoDB;


-- "ACID"
-- A -> Atomicity
-- C -> Consistency
-- I -> Isolation
-- D -> Durability

-- start transaction
SET AUTOCOMMIT = 0;
-- very important because only when you close the autocommit to 0

-- delete the transaction process
DROP PROCEDURE IF EXISTS process_touhou_transaction;

-- create transaction process
DELIMITER $$

CREATE PROCEDURE process_touhou_transaction()
BEGIN
	START TRANSACTION;

	INSERT INTO touhou (name, species, ability, affiliation) VALUES
	('Reimu Hakurei', 'Human', 'Flight, sealing power', 'Hakurei Shrine'),
	('Marisa Kirisame', 'Human', 'Magic, light-based lasers', 'Kirisame Magic Shop'),
	('Sakuya Izayoi', 'Human', 'Time manipulation', 'Scarlet Devil Mansion');

	SAVEPOINT add_three;

	SELECT * FROM touhou;

	IF (SELECT COUNT(id) FROM touhou) < 4 THEN

		IF (SELECT COUNT(name) FROM touhou WHERE name = "Reimu Hakurei") > 0 THEN
			REPLACE INTO touhou (name, species, ability, affiliation) VALUES
				('Reimu Hakurei', 'Human', 'Fuck, sealing power', 'Hakurei Shrine');
		END IF;

		IF (SELECT COUNT(id) FROM touhou) < 4 THEN 
			INSERT INTO touhou (name, species, ability, affiliation) VALUES
				('Youmu Konpaku', 'Half-human, half-phantom', 'Swordsmanship', 'Hakugyokurou');
				COMMIT;
		ELSE 
			COMMIT;
		END IF;

	ELSE
		ROLLBACK TO add_three;
		COMMIT;

	END IF;

END $$

DELIMITER ;

CALL process_touhou_transaction();

SELECT * FROM touhou;

