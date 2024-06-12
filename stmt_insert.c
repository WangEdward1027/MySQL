//预处理 SQL 语句: 使用MYSQL_STMT结构体插入数据，能防止SQL注入攻击
              //还能模板化SQL语句，提高查询效率

#include <func.h>
#include <mysql/field_types.h>
#include <mysql/mysql.h>
#include <stdlib.h>

int main()
{
    //初始化MYSQL结构体
    MYSQL conn;
    MYSQL * pconn = mysql_init(&conn);
    //连接MySQL服务器
    const char * host = "localhost";   //IP地址
    const char * user = "root";
    const char * passwd = "1234";
    const char * db = "cpp58";
    pconn = mysql_real_connect(pconn, host, user, passwd, db, 0, NULL, 0);
    if(pconn == NULL) {
        printf("error:%s\n", mysql_error(&conn));
        return EXIT_FAILURE;
    }

    //初始化MYSQL_STMT
    MYSQL_STMT * stmt = mysql_stmt_init(pconn);
    if(stmt == NULL) {
        printf("error:%s\n", mysql_error(pconn));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }

    //执行PREPARE操作,将stmt与模板化的sql语句绑定
    const char * sql = "INSERT INTO t_user VALUES(?,?,?)";
    int ret = mysql_stmt_prepare(stmt, sql, strlen(sql));
    if(ret) {
        printf("(%d, %s\n)", mysql_stmt_errno(stmt),
               mysql_stmt_error(stmt));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }

    //获取模板化的sql中占位符的个数
    int count = mysql_stmt_param_count(stmt);
    if(count != 3) {
        printf("(%d, %s\n)", mysql_stmt_errno(stmt),
               mysql_stmt_error(stmt));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }

    //设置参数
    int id = 0;

    char username[20] = {0};
    unsigned long name_len = 0;

    char password[20] = {0};
    unsigned long passwd_len = 0;

    //绑定参数(非常关键)
    MYSQL_BIND bind[3];
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_LONG;
    bind[0].buffer = &id;
    bind[0].is_null = 0;//该参数不为空,设置为0；参数为空，设置为1
    bind[0].length = NULL;//整型数据不需要指定长度缓冲区

    bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[1].buffer = username;//指定字符串的缓冲区的首地址
    bind[1].is_null = 0;
    bind[1].length = &name_len;//指定字符串长度的缓冲区的首地址

    bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[2].buffer = password;
    bind[2].is_null = 0;
    bind[2].length = &passwd_len;
    //执行绑定操作
    ret = mysql_stmt_bind_param(stmt, bind);
    if(ret) {
        printf("(%d, %s\n)", mysql_stmt_errno(stmt),
               mysql_stmt_error(stmt));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }

    //第一次传递参数
    id = 3;
    strcpy(username, "唐三");
    name_len = strlen("唐三");
    strcpy(password, "8888");
    passwd_len = strlen("8888");
    //第一次执行EXECUTE语句
    ret = mysql_stmt_execute(stmt);
    if(ret) {
        printf("(%d, %s\n)", mysql_stmt_errno(stmt),
               mysql_stmt_error(stmt));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }

    printf("QUERY OK, %ld row affected.\n",
           mysql_stmt_affected_rows(stmt));
    //第二次传递参数
    id = 4;
    strcpy(username, "比比东");
    name_len = strlen("比比东");
    strcpy(password, "99999");
    passwd_len = strlen("99999");
    //第二次执行EXECUTE语句
    ret = mysql_stmt_execute(stmt);
    if(ret) {
        printf("(%d, %s\n)", mysql_stmt_errno(stmt),
               mysql_stmt_error(stmt));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }

    printf("QUERY OK, %ld row affected.\n",
           mysql_stmt_affected_rows(stmt));

    mysql_stmt_close(stmt);
    mysql_close(pconn);
    return 0;
}
