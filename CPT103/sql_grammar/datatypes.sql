CREATE DATABASE IF NOT EXISTS `RUNOOD`
CHARACTER SET utf8mb4
COLLATE utf8mb4_general_ci;

USE `runood`;
CREATE TABLE IF NOT EXISTS `testType`(
	-- the tinyInt will only cost for 1 byte;
	`tinyInt` TINYINT UNSIGNED NOT NULL DEFAULT 255,
	-- smallint will cost about 2 bytes;
	`smallInt` SMALLINT	UNSIGNED NOT NULL DEFAULT 65535,
	-- mediumint will cost for 3 bytes;
	`mediumInt` MEDIUMINT UNSIGNED NOT NULL DEFAULT 16777215,
	-- the INTEGER or INT will cost about 4 bytes
	`int` INT UNSIGNED NOT NULL DEFAULT 4294967295,
	-- the BIGINT will cost 8 bytes
	`bigint` BIGINT UNSIGNED NOT NULL DEFAULT 1,
	-- the float will cost 4 bytes
	`float` FLOAT UNSIGNED NOT NULL DEFAULT 1,
	-- the double takes 8 bytes
	`double` DOUBLE UNSIGNED NOT NULL DEFAULT 1.1,
	-- the decimal should be like (4, 2) -> 42.22
	`dec` DEC(10, 2) NOT NULL DEFAULT 1.11
);

-- show the tables's column
SELECT "";
SELECT "show table columns";

SHOW COLUMNS FROM runood.testtype;

SELECT "";
SELECT "the String things";

CREATE TABLE IF NOT EXISTS `testString` (
    -- CHAR: 定长字符串，存储长度为 100 字符
	    `char` CHAR(100),
	    
	-- VARCHAR: 变长字符串，最多可存储 100 字符，未使用的存储空间不占用
		`varchar` VARCHAR(100),
    
    -- TINYBLOB: 存储二进制数据，最大大小为 255 字节
	    `tinyblob` TINYBLOB,
    
    -- TINYTEXT: 存储文本数据，最大大小为 255 字符
	    `tinytext` TINYTEXT,
    
    -- BLOB: 存储二进制数据，最大大小为 65,535 字节
	    `blob` BLOB,
    
    -- TEXT: 存储文本数据，最大大小为 65,535 字符
	    `text` TEXT,
    
    -- MEDIUMBLOB: 存储二进制数据，最大大小为 16,777,215 字节
	    `mediumblob` MEDIUMBLOB,
    
    -- MEDIUMTEXT: 存储文本数据，最大大小为 16,777,215 字符
	    `mediumtext` MEDIUMTEXT,
    
    -- LONGBLOB: 存储二进制数据，最大大小为 4,294,967,295 字节
	    `longblob` LONGBLOB,
    
    -- LONGTEXT: 存储文本数据，最大大小为 4,294,967,295 字符
	    `longtext` LONGTEXT
);
-- ATTENTION!!!
-- NO (100) FOR THE UNSIGNED TYPE UPPER

SHOW COLUMNS FROM runood.teststring;

SELECT "";
SELECT "EMUM and SET";

CREATE TABLE IF NOT EXISTS `enumtable`(
	-- the ENUM means single selection from this column
	`enum_menu` ENUM('activate', 'die', 'stirb') NOT NULL DEFAULT 'activate',
	-- the set means multiset in the set
	-- if you want default multiple values, use 'value1,value2' without space
	`set_menu` SET('test1', 'test2', 'test3') NOT NULL DEFAULT 'test1,test2'
)
SELECT "DONE";
SHOW COLUMNS FROM runood.enumtable;



-- the time things
SELECT "";
SELECT "time things";
CREATE TABLE IF NOT EXISTS `TIMETABLE` (
    -- `DATE`: Stores a date in the format YYYY-MM-DD
	-- The default value is '1000-01-01' which is the earliest date allowed by MySQL
	`date` DATE NOT NULL DEFAULT '1000-01-01',

	-- `TIME`: Stores a time in the format HH:MM:SS
	-- The default value is the time when the record is inserted ('CURRENT_TIME')
	`time` TIME NOT NULL DEFAULT '00:00:00',

	-- `YEAR`: Stores a year in the format YYYY
	-- The default value is '2024', which is the year when this table was created
	`year` YEAR NOT NULL DEFAULT '2024',

	-- `DATETIME`: Stores both date and time in the format YYYY-MM-DD HH:MM:SS
	-- The default value is the current date and time when the record is inserted
	`datetime` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,

	-- `TIMESTAMP`: Stores both date and time, typically for tracking changes
	-- The default value is the current timestamp when the record is inserted
	-- It automatically updates to the current timestamp
	-- whenever the record is updated
	`timestamp` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP 
	ON UPDATE CURRENT_TIMESTAMP
);


-- the special data types

SELECT "";
SELECT "spatial_data things";
CREATE TABLE IF NOT EXISTS `spatial_data` (
    -- 存储通用几何数据
    `geometry_field` GEOMETRY NOT NULL,
    
    -- 存储点的坐标
    `point_field` POINT NOT NULL,
    
    -- 存储线段
    `linestring_field` LINESTRING NOT NULL,
    
    -- 存储多边形
    `polygon_field` POLYGON NOT NULL,
    
    -- 存储多个点
    `multipoint_field` MULTIPOINT NOT NULL,
    
    -- 存储多个线段
    `multilinestring_field` MULTILINESTRING NOT NULL,
    
    -- 存储多个多边形
    `multipolygon_field` MULTIPOLYGON NOT NULL,
    
    -- 存储不同类型的几何集合
    `geometrycollection_field` GEOMETRYCOLLECTION NOT NULL
);

SHOW COLUMNS FROM runood.spatial_data;

SELECT "FUCKING DONE!!!"
