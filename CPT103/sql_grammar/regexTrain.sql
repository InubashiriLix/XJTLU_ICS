-- well, may be you have learnt something about regex expression
-- in language like php (php is the best lang in the world.py) 
-- java and python
-- you could learn the regex in MYSQL better

-- byw, the like can do some same things

USE runood;

-- find the content in the html file
-- SELECT content
-- FROM text_samples
-- WHERE description LIKE "HTML%"
-- AND CONTENT REGEXP "<.*?>";

-- find the column witch has the name, age, email
-- SELECT content 
-- FROM text_samples
-- WHERE content REGEXP "^Name: [a-zA-Z0-9 ]+, Age: [0-9]+, Phone:";

-- find the paragragh
-- SELECT description, content
-- FROM text_samples
-- WHERE content REGEXP "^Lorem";

-- find the path in the linux sys
-- SELECT description, content 
-- FROM text_samples
-- WHERE content REGEXP "/[^:]+";
-- WHERE content REGEXP "^/";

-- find the url
-- SELECT description, content 
-- FROM text_samples
-- WHERE content REGEXP "https://[a-zA-Z0-9]*";

-- find the IP
-- SELECT description, content 
-- FROM text_samples
-- WHERE content REGEXP "\\b[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\b";

-- find verification code
-- SELECT description, content
-- FROM text_samples
-- WHERE content REGEXP "is \\b\\d{2,}\\b";

-- Date, time
-- SELECT description, content 
-- FROM text_samples
-- WHERE content REGEXP "\\b[0-9]{4}\\-[0-9]{2}\\-[0-9]{2}\\b";

-- SELECT description, content 
-- FROM text_samples
-- WHERE content REGEXP "\\b[0-9]{4}-[0-9]{2}-[0-9]{2},\\sTime:\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\b";

-- find the command in the mysql
SELECT description, content
FROM text_samples
WHERE content REGEXP "\\bSELECT\\b";

-- linux 系统路径
-- SELECT description, content
-- FROM text_samples
-- WHERE content REGEXP '/[^:]+';

-- 匹配 IP 地址
-- SELECT description, content
-- FROM text_samples
-- WHERE content REGEXP '\\b[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\b";

-- 提取 URL
-- SELECT description, content
-- FROM text_samples
-- WHERE content REGEXP 'https?://\\S+';

-- 查找电话号码
-- SELECT description, content
-- FROM text_samples
-- WHERE content REGEXP '\\([0-9]{3}\\) [0-9]{3}-[0-9]{4}';

-- 匹配订单编号
-- SELECT description, content
-- FROM text_samples
-- WHERE content REGEXP '\\b[0-9]{6}\\b';

-- 提取 SQL 查询中的表名
-- SELECT description, content
-- FROM text_samples
-- WHERE content REGEXP 'FROM\\s+\\w+';
