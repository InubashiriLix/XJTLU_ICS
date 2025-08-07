USE runood;

-- use the unique constrain adn the 
-- priamry key to constraint the duplicated data

CREATE TEMPORARY TABLE DUPLICATED(
	first_name CHAR(20),
	last_name CHAR(20),
	sex VARCHAR(50),
	-- set the double primary key to to constraint
	-- attention that the field that we apply the double 
	-- priamry key should be NOT NULL!!!
	PRIMARY KEY(first_name, last_name)
);

-- the difference between the 
-- INSERT IGNORE INTO & INSERT INTO 
-- is that the insert ignore can ignore 
-- the duplicated data...


INSERT INTO duplicated (first_name, last_name, sex) VALUES
('testName1', 'testName1', 'male'),
('testName2', 'testName2', 'female');

SELECT * FROM duplicated;

INSERT IGNORE INTO duplicated VALUES
('testName1', 'testName1', 'male'),
('testName2', 'testName2', 'female'),
('testName1', 'testName2', 'male'),
('testName2', 'testName1', 'male');

SELECT * FROM duplicated;

-- if we want a table with both the first_name and last_name 
-- are unique

DROP TABLE IF EXISTS duplicated2;

CREATE TEMPORARY TABLE duplicated2 (
	first_name CHAR(20) NOT NULL,
	last_name CHAR(20) NOT NULL,
	male BOOLEAN NOT NULL DEFAULT FALSE,
	UNIQUE (last_name, first_name)
);

INSERT INTO duplicated2 (first_name, last_name, male) VALUES
('testName1', 'testName1', FALSE),
('testName2', 'testName2', FALSE);

SELECT * FROM duplicated2;

INSERT IGNORE INTO duplicated2 VALUES
('testName1', 'testName1', FALSE),
('testName2', 'testName2', FALSE),
('testName1', 'testName2', FALSE),
('testName2', 'testName1', FALSE);

SELECT * FROM duplicated2;



-- statistic the duplicated data
-- create a table with duplicated dataVING
CREATE TEMPORARY TABLE duplicated3
(
	first_name CHAR(20) NOT NULL,
	last_name CHAR(20) NOT NULL,
	male BOOLEAN NOT NULL DEFAULT FALSE
);


INSERT INTO duplicated3 (first_name, last_name, male) VALUES
('testName1', 'testName1', FALSE),
('testName2', 'testName2', FALSE);

SELECT * FROM duplicated3;

INSERT INTO duplicated3 VALUES
('testName1', 'testName1', FALSE),
('testName2', 'testName2', FALSE),
('testName1', 'testName2', FALSE),
('testName2', 'testName1', FALSE);

SELECT * FROM duplicated3;
-- duplicated data added!

SELECT COUNT(*) as repetitions, first_name, last_name
FROM duplicated3
GROUP BY first_name, last_name
-- having is for processing result that has been processed
-- in the SELECT sentences
HAVING repetitions > 1;


-- DELETE THE DUPLICATED DATA
-- 创建一个临时表保存去重后的数据
-- CREATE TEMPORARY TABLE temp AS
-- SELECT first_name, last_name, male
-- FROM duplicated3
-- GROUP BY first_name, last_name, male;

-- -- 删除原表 duplicated3
-- DROP TABLE duplicated3;

-- -- 将临时表重命名为 duplicated4
-- ALTER TABLE temp RENAME TO duplicated4;

-- -- 查看新表的去重数据
-- SELECT * FROM duplicated4;

-- delete the duplicated data in another wa
DELETE FROM duplicated3
WHERE (first_name, last_name) IN (
	SELECT first_name, last_name
	FROM (
		SELECT first_name, last_name
		FROM duplicated3
		GROUP BY first_name, last_name
		HAVING COUNT(*) > 1
	) AS temp_duplicates
);
