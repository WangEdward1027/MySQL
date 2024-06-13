-- 向表格employee中插入一千万条员工数据, 使用 【存储过程】 + 【显式事务】
-- | id | name | email | salary | sex | birth |

DROP PROCEDURE test1;
CREATE PROCEDURE test1(IN num INT)
BEGIN
	DECLARE x INT;
	SET x = 0;
	START TRANSACTION;	-- 事务
	WHILE x < num DO
			INSERT INTO employee(id, name,email,salary,sex,birth) 
			VALUES(x+1, CONCAT('Edward',x+1), CONCAT('123',x+1,'@qq.com'),10000+1000*(x+1), 'Male',CONCAT('200',x%10,'-01-01'));
			SET x = x + 1;
	END WHILE;
	COMMIT;	-- 提交
	SELECT * FROM employee WHERE id <= 10000000;
END;

CALL test1(10000000);
