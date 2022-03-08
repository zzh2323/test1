#include "msg_send_service.h"

extern Link client_head;
void send_msg_handle(int sockfd, const char *content)
{
    struct msg_define msg_struct;
    memset(&msg_struct, 0, sizeof(msg_struct));
    memcpy(&msg_struct, content, sizeof(msg_struct));

    printf("client %d send %s msg :%s  is send_all:%d\n", sockfd, msg_struct.user_name, msg_struct.msg, msg_struct.is_send_all);

    //找到用户发送的sockfd
    int fds[1024];
    char send_user[1024] = "\0";

    Link p = client_head;
    int count = 0;
    int for_count = 0;

    while (p != NULL)
    {
        printf("upline client: ip %s port %s fd:%d\n", p->user_name, p->pass_word, p->sockfd);

        if (p->sockfd == sockfd) //找到需要发送的用户
        {
            strcpy(send_user, p->user_name);
        }
        if (msg_struct.is_send_all == 1)
        {
            fds[count] = p->sockfd;
        }
        else
        {
            //找到目标用户
            if (0 == strcmp(p->user_name, msg_struct.user_name))
            {
                printf("compared send user %s\n", p->user_name);
                fds[0] = p->sockfd;
                for_count = 1;
            }
        }
        count++;
        p = p->next;
    }

    if (1 == user_is_silence_db(send_user))
    {
        struct msg_define silence_struct;
        memset(&silence_struct, 0, sizeof(silence_struct));
        strcpy(silence_struct.msg, "You have been silenced Now can't send message!");
        strcpy(silence_struct.user_name, send_user);
        char s_buffer[sizeof(silence_struct)] = "\0";
        memcpy(s_buffer, &silence_struct, sizeof(silence_struct));
        int ret = send(sockfd, s_buffer, sizeof(silence_struct), 0);
        if (-1 == ret)
        {
            perror("send client msg error!\n");
        }
    }
    else if (1 == user_is_kick_clientlist(send_user))
    {
        struct msg_define kick_struct;
        memset(&kick_struct, 0, sizeof(kick_struct));
        strcpy(kick_struct.msg, "You have been kicked off the line Now can't send message!");
        strcpy(kick_struct.user_name, send_user);
        char s_buffer[sizeof(kick_struct)] = "\0";
        memcpy(s_buffer, &kick_struct, sizeof(kick_struct));
        int ret = send(sockfd, s_buffer, sizeof(kick_struct), 0);
        if (-1 == ret)
        {
            perror("send client msg error!\n");
        }
    }
    else
    {
        struct msg_define no_silence_struct;
        memset(&no_silence_struct, 0, sizeof(no_silence_struct));
        strcpy(no_silence_struct.msg, "Send success");
        strcpy(no_silence_struct.user_name, send_user);
        char ss_buffer[sizeof(no_silence_struct)] = "\0";
        memcpy(ss_buffer, &no_silence_struct, sizeof(no_silence_struct));
        int ret = send(sockfd, ss_buffer, sizeof(no_silence_struct), 0);
        if (-1 == ret)
        {
            perror("send client msg error!\n");
        }



        for_count = (msg_struct.is_send_all == 1) ? count : for_count;

        for (size_t i = 0; i < for_count; i++)
        {
            int fd = fds[i];
            struct msg_define s_msg_struct;
            memset(&s_msg_struct, 0, sizeof(s_msg_struct));
            struct msg_header header = get_init_header(MSG_SEND, sizeof(s_msg_struct));

            strcpy(s_msg_struct.msg, msg_struct.msg);
            strcpy(s_msg_struct.user_name, send_user);
            s_msg_struct.is_send_all = msg_struct.is_send_all;

            char s_buffer[sizeof(s_msg_struct) + sizeof(header)] = "\0";
            memcpy(s_buffer, &header, sizeof(header));
            memcpy(s_buffer + sizeof(header), &s_msg_struct, sizeof(s_msg_struct));

            //发送消息
            int s_ret = send(fd, s_buffer, sizeof(s_msg_struct) + sizeof(header), 0);
            if (-1 == s_ret)
            {
                perror("send client msg error!\n");
            }
        }

        
    }
}

void silence_someone(int sockfd, const char *content)
{
    struct user_struct user;
    memset(&user, 0, sizeof(user));
    memcpy(&user, content, sizeof(user));
    Link p = client_head;
    char silencename[1024];
    strcpy(silencename, user.user_name);
    printf("%s\n", silencename);
    while (p != NULL)
    {
        if (strcmp(p->user_name,user.user_name) == 0) //找到需要禁言的用户
        {
            strcpy(silencename, p->user_name);
            break;
        }
        p = p->next;   

    }

    char final_sql[1024] = "\0";
    char *sql = "update users set SILENCE_YORN = 'Y' where USER_NAME = '%s';";
    sprintf(final_sql, sql, silencename);
    zchat_exec_sql(final_sql, NULL, NULL, NULL);
}

void cancel_silence_someone(int sockfd, const char *content)
{
    struct user_struct user;
    memset(&user, 0, sizeof(user));
    memcpy(&user, content, sizeof(user));
    printf("%s\n",user.user_name);
    Link p = client_head;
    char silencename[1024];
    strcpy(silencename, user.user_name);
    printf("%s\n", silencename);
    while (p != NULL)
    {
        if (strcmp(p->user_name,user.user_name) == 0) //找到需要解禁言的用户
        {
            strcpy(silencename, p->user_name);
            break;
        }
        p = p->next;   
    }
    char final_sql[1024] = "\0";
    char *sql = "update users set SILENCE_YORN = 'N' where USER_NAME = '%s';";
    sprintf(final_sql, sql, silencename);
    zchat_exec_sql(final_sql, NULL, NULL, NULL);
}

void kick_someone(int sockfd, const char *content)
{
    struct user_struct user;
    memset(&user, 0, sizeof(user));
    memcpy(&user, content, sizeof(user));
    Link p = client_head;
    int kick_sockfd = 0;
    printf("%s\n",user.user_name);

    while (p != NULL)
    {
        printf("%s\n",p->user_name);
        if (strcmp(user.user_name,p->user_name) == 0) //找到需要踢的用户
        {
            kick_sockfd = p->sockfd;
            printf("compared kick user!\n");
            break;
        }
        // else
        // {
        //     //找不到目标用户
        //     printf("目标不在在线用户链表中,无法踢出!\n");
        // }
        p = p->next;
    }
    delete_node(&client_head, kick_sockfd);
    send_upline_users(-1, 1);
    printf("kick success!\n");
}