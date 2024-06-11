//mysql注入,盗取数据库所有信息

#include <func.h>
#include <mysql/mysql.h>
#include <stdlib.h>

int main()
{
    MYSQL conn;
    MYSQL* pconn = mysql_init(&conn);

    //与MySQL服务器建立连接
    pconn = mysql_real_connect(&conn, "localhost", "root", "1234", "cpp58", 0, NULL, 0);
    if(pconn == NULL) {
        printf("%s\n", mysql_error(&conn));
        return EXIT_FAILURE;
    }
    //进行查询操作
    const char * query = "select * from Student where name= '' OR '1'='1';-- ' and password='1123'";  //mysql注入语句
    //mysql_query无法解决SQL注入的问题,会造成数据库的信息泄露
    int ret = mysql_query(pconn, query);
    if(ret != 0) {
        printf("(%d, %s)\n",
               mysql_errno(pconn),
               mysql_error(pconn));
        return EXIT_FAILURE;
    }
    //查询成功的情况下,获取到结果集
    //使用mysql_store_result时，倾向于数据量较小的情况
    //如果数据量很大（百万级别）不适合使用
    MYSQL_RES * result = mysql_store_result(pconn);
    if(result) {
        //先获取属性列的信息
        MYSQL_FIELD * pfield = mysql_fetch_fields(result);
        //获取结果集的行列信息
        int cols = mysql_num_fields(result);
        int rows = mysql_num_rows(result);
        printf("cols: %d, rows:%d\n", cols, rows);

        //打印一行的属性列信息
        for(int i = 0; i < cols; ++i) {
            printf("%s\t", pfield[i].name);
        }
        printf("\n");

        //打印具体的每一行数据
        MYSQL_ROW row;//MSYQL_ROW本质上是一个char**
        while((row = mysql_fetch_row(result))!= NULL) {
            for(int i = 0; i < cols; ++i) {
                printf("%s\t", row[i]);
            }
            printf("\n");
        }
    }
    //释放结果集中的数据
    mysql_free_result(result);
    //关闭连接
    mysql_close(pconn);

    return 0; ;
}
