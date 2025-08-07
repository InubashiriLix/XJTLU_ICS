USE runood;

-- Mr.killmyself
CREATE TEMPORAY TABLE timetable(
	time TIME,
	date DATE,
	datetime DATETIME,
	timestamp TIMESTAMP,
	year YEAR
)

INSERT INTO timetable 

-- SELECT hire_date FROM employees;
SELECT ADDDATE(hire_date, 3) AS new_hire_date, hire_date
FROM employees
LIMIT 4;


