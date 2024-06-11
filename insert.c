//insert.c

#include <func.h>
#include <mysql/mysql.h>
#include <stdlib.h>

int main()
{
    //初始化MYSQL连接
    MYSQL conn;
    MYSQL* pconn = mysql_init(&conn);

    //与MySQL服务器建立连接
    pconn = mysql_real_connect(&conn, "192.168.248.137", "root", "1234", "cpp58", 0, NULL, 0);
    if(pconn == NULL) {
        printf("%s\n", mysql_error(&conn));
        return EXIT_FAILURE;
    }

    //进行查询操作
    const char * query = "INSERT INTO Student VALUES(6, 'Kris', '1999-01-01',90,100,80)";
    int ret = mysql_query(pconn, query);
    if(ret !=0) {
        printf("(%d, %s)\n", mysql_errno(pconn), mysql_error(pconn));
        return EXIT_FAILURE;
    } else {
        printf("QUERY OK, %llu row afftected.\n", mysql_affected_rows(pconn));
    }

    mysql_close(pconn);
    return 0;
}
