//预处理SQL语句：SELECT查询操作，且能防止SQL注入攻击 (使用MYSQL_STMT结构体)

#include <func.h>
#include <mysql/field_types.h>
#include <mysql/mysql.h>
#include <stdlib.h>

int main()
{
    MYSQL conn;
    MYSQL * pconn = mysql_init(&conn);

    pconn = mysql_real_connect(pconn,"localhost","root","1234","cpp58",3306,NULL,0);
    if(pconn == NULL) {
        printf("%s\n", mysql_error(&conn));
        return EXIT_FAILURE;
    }

    //初始化MYSQL_STMT
    MYSQL_STMT * stmt = mysql_stmt_init(pconn);
    if(stmt == NULL) {
        printf("(%d,%s)\n", mysql_errno(pconn),
               mysql_error(pconn));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }

    //执行PREPARE操作
    const char * sql = "SELECT * FROM t_user WHERE id > ?";
    int ret = mysql_stmt_prepare(stmt, sql, strlen(sql));
    if(ret) {
        printf("(%d, %s)\n", mysql_stmt_errno(stmt),
               mysql_stmt_error(stmt));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }

    //获取占位符的个数
    int count = mysql_stmt_param_count(stmt);
    if(count != 1) {
        printf("(%d, %s)\n", mysql_stmt_errno(stmt),
               mysql_stmt_error(stmt));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }

    //设置输入参数
    int id = 0;
    MYSQL_BIND bind;
    memset(&bind, 0, sizeof(bind));

    //绑定参数
    bind.buffer_type = MYSQL_TYPE_LONG;
    bind.buffer = &id;
    bind.is_null = 0;
    bind.length = NULL;//整型数据不需要设置length

    //执行绑定操作
    ret = mysql_stmt_bind_param(stmt, &bind);
    if(ret) {
        printf("(%d, %s)\n", mysql_stmt_errno(stmt),
               mysql_stmt_error(stmt));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }

    //设置参数的值
    id = 1;
    //执行EXECUTE操作
    ret = mysql_stmt_execute(stmt);
    if(ret) {
        printf("(%d, %s)\n", mysql_stmt_errno(stmt),
               mysql_stmt_error(stmt));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }

    //先获取field字段
    MYSQL_RES * res = mysql_stmt_result_metadata(stmt);
    if(res) {
        MYSQL_FIELD * field = NULL;
        //在while循环中不断获取下一个field
        int cols = mysql_num_fields(res);
        printf("cols: %d\n", cols);
        field = mysql_fetch_fields(res);
        for(int j = 0; j < cols; ++j) {
            printf("%s\t", field[j].name);
        }
        /* while((field = mysql_fetch_field(res))!= NULL) { */
        /*     printf("%s\t", field->name); */
        /* } */
        printf("\n");
    }

    //设置输出参数(后续获取的每一行信息都要写到这3个字段中)
    int res_id;
    char res_username[20] = {0};
    char res_password[20] = {0};
    MYSQL_BIND res_bind[3];
    memset(res_bind, 0, sizeof(res_bind));
    //绑定输出参数
    res_bind[0].buffer_type = MYSQL_TYPE_LONG;
    res_bind[0].buffer = &res_id;
    res_bind[0].buffer_length = sizeof(int);

    res_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    res_bind[1].buffer = res_username;
    res_bind[1].buffer_length = sizeof(res_username);//确定值,空间要大

    res_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    res_bind[2].buffer = res_password;
    res_bind[2].buffer_length = sizeof(res_password);//确定值
    //执行绑定操作
    ret = mysql_stmt_bind_result(stmt, res_bind);
    if(ret) {
        printf("(%d, %s)\n", mysql_stmt_errno(stmt),
               mysql_stmt_error(stmt));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }
    //再获取数据信息
    ret = mysql_stmt_store_result(stmt);
    if(ret) {
        printf("(%d, %s)\n", mysql_stmt_errno(stmt),
               mysql_stmt_error(stmt));
        mysql_close(pconn);
        return EXIT_FAILURE;
    }
    //真正获取数据时，不是用MYSQL_RES来操作了
    while(1) {
        //每当调用一次mysql_stmt_fetch函数
        //res_bind中的绑定的三字字段就会被填充
        int status = mysql_stmt_fetch(stmt);
        if(status == 1 || status == MYSQL_NO_DATA) {
            break;
        }
        //打印一行数据
        printf("%d\t%s\t%s\n", res_id, res_username, res_password);
    }

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);
    mysql_close(pconn);

    return 0;
}
