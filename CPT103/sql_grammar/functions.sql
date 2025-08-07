use runood;

-- SELECT ASCII("abc");
-- 97

-- SELECT CHAR_LENGTH("abcdefg"); 
-- 7

-- SELECT CHARACTER_LENGTH ("ABC");
-- 3

-- SELECT CONCAT("abc", "cde", "efg"); 
-- abccdeefg

-- abc,bcd,cde
-- SELECT CONCAT_WS(",", "abc","bcd","cde");



-- SELECT FIELD("c", "a", "b", "c");
-- 3

-- SELECT FIND_IN_SET("c", "a,b,c");
-- 3

-- SELECT LOCATE("a", "cba"); 
-- 3

-- SELECT POSITION("abc" IN "cdabccd");
-- 3


-- SELECT FORMAT(202.00123, 3);
-- 202.001


-- SELECT INSERT("abcde", 3, 3, "eee");
-- abeee
-- "abc -> original str, 3 -> start subsittution ptr
-- 3 -> length of substitution
-- "eee" object str

-- SELECT LCASE("ABC"); 
-- abc

-- SELECT LEFT("abcde", 3);
-- abc

-- SELECT RIGHT("abcde", 3);
-- cde

-- SELECT LPAD("abc", 5, "xx");
-- 在字符串 s1 的开始处填充字符串 s2，使字符串长度达到 len

-- SELECT RPAD("abc" 5, "x");
-- abcxx

SELECT LTRIM("  abc");
-- 去掉字符串 s 开始处的空。

SELECT RTRIM("abc  ");


SELECT MID("abcdefg", 3, 2);
-- cd

SELECT REPLACE("fadsfABCasdfd", "ABC", "SDF");
-- fadsfSDFasdfd

SELECT REVERSE("MNCW");

SELECT REPEAT("a", 3);
-- "aaa"

SELECT CONCAT("abc", SPACE(10), "cba");
-- "          "

SELECT STRCMP("abc", "abc");
-- 0

SELECT STRCMP("abc", "bcd");

SELECT STRCMP("bcd", "abc");

SELECT SUBSTR("abcdefg", 4, 3);
SELECT SUBSTRING("abcdefg", 4, 3);
-- def

SELECT SUBSTRING_INDEX("a,b,c", ",", 1);
-- a
SELECT SUBSTRING_INDEX("a,b,c", ",", -1);
-- c

SELECT TRIM("  abc  ");

SELECT UCASE("bacs");
-- BACS

SELECT UPPER("abc");
-- ABC
