SOURCE E:/SQL_LEARNING/createExampleTable.sql;

USE runood;

SELECT id, salary
INTO OUTFILE 'salary.csv'
FIELDS TERMINATED BY ','
LINES TERMINATED BY '\n'
FROM employees;

