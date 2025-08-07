SOURCE createExampleTable.sql

SELECT "";
SELECT * FROM orders;

SELECT "";

USE runood;

-- delete with conditions
DELETE FROM orders
WHERE id = 1;

SELECT "";
SELECT * FROM orders;

-- delete all the orders with money greater than 300
DELETE FROM orders
WHERE money > 300;

SELECT "";
SELECT * FROM orders;


-- delete all the things
DELETE FROM orders;

SELECT "";
SELECT * FROM orders;
