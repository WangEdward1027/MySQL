//read2.c:从数据库读取数据
//使用mysql_use_result时,并不存储结果集,调用一次才拉取一行
//用于数据量较大的情况

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
    const char * query = "select * from Student";
    int ret = mysql_query(pconn, query);
    if(ret != 0) {
        printf("(%d, %s)\n",
               mysql_errno(pconn),
               mysql_error(pconn));
        return EXIT_FAILURE;
    }
    //查询成功的情况下,获取到结果集
    //mysql_use_result并不存储结果集
    MYSQL_RES * result = mysql_use_result(pconn);
    if(result) {
        //先获取属性列的信息
        MYSQL_FIELD * pfield = mysql_fetch_fields(result);
        //获取结果集的行列信息
        int cols = mysql_num_fields(result);
        int rows = mysql_num_rows(result);
        //使用mysql_use_result时，MYSQL_RES是有值的
        //rows为0,不是一个大于0的值
        printf("cols: %d, rows:%d\n", cols, rows);

        //打印一行的属性列信息
        for(int i = 0; i < cols; ++i) {
            printf("%s\t", pfield[i].name);
        }
        printf("\n");

        //打印具体的每一行数据
        MYSQL_ROW row;//MSYQL_ROW本质上是一个char**
        //当使用了mysql_use_result函数之后，每一次
        //调用mysql_fetch_row才真正传输数据
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
