#include "register_service.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "client_list.h"
#include <stdlib.h>
#include "upline_user_service.h"
#include <time.h>
#include <stddef.h>
#include "chat_dao.h"
#include "common_utils.h"
#include "msg_define.h"

extern Link client_head;
extern char name[1024];

//注册

void register_handle(int sockfd, const char *content)
{
    if (content == NULL)
        return;

    struct user_struct user_info;
    memcpy(&user_info, content, sizeof(struct user_struct));

    struct server_replay replay;
    memset(&replay, 0, sizeof(replay));
    struct msg_header header = get_init_header(REGISTER_REPLAY, sizeof(replay));

    char current[1024] = "\0";
    struct timespec time;
    clock_gettime(0, &time); //获取相对于1970到现在的秒数
    struct tm nowTime;
    localtime_r(&time.tv_sec, &nowTime);
    sprintf(current, "%04d/%02d/%02d", nowTime.tm_year + 1900, nowTime.tm_mon, nowTime.tm_mday);

    int is_exist = user_is_exit_db(user_info);
    if (-1 != is_exist)
    {
        // 用户已经存在
        replay.code = CODE_ERROR;
        strcpy(replay.error_msg, "user is exist please change your user name!");
    }
    else
    {
        // 插入数据库
        char final_sql[1024] = "\0";
        replay.code = SUCCESS;
        char *sql = "insert into users (USER_NAME,USER_PASS,REGISTER_TIME,SILENCE_YORN,ADMIN_YORN) values ('%s','%s','%s','%s','%s');";
        //char *sql = "insert into users (USER_NAME,USER_PASS,REGISTER_TIME,OPT) values ('%s','%s','%s','%s');";
        sprintf(final_sql, sql, user_info.user_name, user_info.pass_word, current,"N","N");
        //sprintf(final_sql, sql, user_info.user_name, user_info.pass_word, current, user_info.opt);
        zchat_exec_sql(final_sql, NULL, NULL, NULL);
    }

    char s_buffer[sizeof(header) + sizeof(replay)] = "\0";
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &replay, sizeof(replay));

    if (-1 == send(sockfd, s_buffer, sizeof(header) + sizeof(replay), 0))
    {
        perror("send login replay to user error!\n");
    }
}

//登录
void user_login_handle(int sockfd, const char *content)
{

    if (content == NULL)
        return;
    struct user_struct user_info;
    memcpy(&user_info, content, sizeof(struct user_struct));
    struct server_replay replay;
    struct msg_header header = get_init_header(LOGIN_REPLAY, sizeof(replay));
    memset(&replay, 0, sizeof(replay));

    //用户是否在线
    Link p = client_head;
    int is_upline = -1;
    while (p != NULL)
    {
        if (0 == strcmp(p->user_name, user_info.user_name))
        {
            replay.code = CODE_ERROR;
            strcpy(replay.error_msg, "your account has already upline!\n");
            is_upline = 1;
            break;
        }
        p = p->next;
    }

    //用户存在
    int is_exist = -1;
    if (-1 == is_upline)
    {
        is_exist = user_is_exit_db(user_info);
        if (-1 == is_exist)
        {
            //用户登录失败
            replay.code = CODE_ERROR;
            strcpy(replay.error_msg, "please check your count!");
        }
        else
        {
            replay.code = CODE_SUCCESS;
        }
    }

    //构造消息
    char s_buffer[sizeof(header) + sizeof(replay)] = "\0";
    memcpy(s_buffer, &header, sizeof(header));
    memcpy(s_buffer + sizeof(header), &replay, sizeof(replay));

    if (-1 == send(sockfd, s_buffer, sizeof(header) + sizeof(replay), 0))
    {
        perror("send login replay to user error!\n");
    }

    if (is_exist != -1 && is_upline == -1) //登陆成功
    {
        Link new_node;
        create_node(&new_node);
        new_node->sockfd = sockfd;
        memcpy(new_node->user_name, user_info.user_name, 32);
        memcpy(new_node->pass_word, user_info.pass_word, 32);

        //插入
        insert_list_head(&client_head, new_node);

        //返回客户端目前在线的用户
        send_upline_users(-1, 1);
    }
}

int user_is_exit_db(struct user_struct user)
{
    char **query_res;

    char final_sql[1024] = "\0";

    char *sql = "select * from users where USER_NAME ='%s' and USER_PASS ='%s';";
    sprintf(final_sql,sql, user.user_name, user.pass_word);

    printf("user Login query sql : %s\n", final_sql);

    int row;
    int col;

    zchat_query_table(final_sql, query_res, &row, &col, NULL);

    if (row > 0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int user_is_silence_db(char *user)
{
    char **query_res;

    char final_sql[1024] = "\0";

    char *sql = "select * from users where USER_NAME ='%s' and SILENCE_YORN ='%s';";
    sprintf(final_sql,sql, user, "Y");

    printf("user Login query sql : %s\n", final_sql);

    int row;
    int col;

    zchat_query_table(final_sql, query_res, &row, &col, NULL);

    if (row > 0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int user_is_kick_clientlist(char *user)
{
    Link p = client_head;

    while (p != NULL)
    {
        if (0 == strcmp(p->user_name, user))
        {
            return 0;
            break;
        }
        p = p->next;
    }
    return 1;
}
