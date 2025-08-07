USE runood;

CREATE TEMPORARY TABLE example_table (
	id INT AUTO_INCREMENT PRIMARY KEY,
	name VARCHAR(50)
) AUTO_INCREMENT = 2;

INSERT INTO example_table VALUES 
(DEFAULT, 'name1'),
(DEFAULT, 'name2');

SELECT * FROM example_table \G;

-- USE the LAST_INSERT_ID to get the last auto_increment value
-- SELECT LAST_INSERT_ID();
-- SHOW TABLE STATUS LIKE 'example_table';


-- USE the fucking AUTO_INCREMENT in the mysql
CREATE TEMPORARY TABLE insect (
	id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
	name VARCHAR(255) NOT NULL,
	date DATE NOT NULL,
	origin VARCHAR(30) NOT NULL
);

INSERT INTO insect (id, name, date, origin) VALUES 
(DEFAULT, 'housefly', '2001-09-10', 'kitchen'),
(DEFAULT, 'millipede', '2001-09-10', 'driveway'),
(DEFAULT, 'graasshopper', '2003-09-10', 'stirb');

SELECT * FROM insect;

-- get the value of auto_increment
-- use the
SELECT LAST_INSERT_ID();
-- SHOW TABLE STATUS LIKE 'insect';
