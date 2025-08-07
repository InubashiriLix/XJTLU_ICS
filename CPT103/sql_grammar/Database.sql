-- create DATABASE `runoob`;
-- SHOW DATABASES;
-- mysqladmin -u root -p=Lix0123456789

-- create a database there
CREATE DATABASE IF NOT EXISTS runood1
CHARACTER SET utf8mb4
COLLATE utf8mb4_general_ci;

-- create with default params settings 
-- in the mysqladmin mode
create database IF NOT EXISTS `runood2`;

-- because the mysqladmin is not avaliable there
-- we use the first commands there

SHOW DATABASES;

-- delete the databases
-- use the drop there 
DROP DATABASE runood1;

SHOW DATABASES;
-- sucess there 
-- this operataion can not be recovered.

