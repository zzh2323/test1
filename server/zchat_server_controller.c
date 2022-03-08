#include "zchat_server_controller.h"
#include "common_utils.h"
#include "msg_define.h"
#include "upline_user_service.h"
#include <stdio.h>
#include "msg_send_service.h"
#include "register_service.h"
#include "chat_dao.h"
#include "client_list.h"
#include "z_block_server.h"
#include "file_send_service.h"

//全局客户端链表
Link client_head = NULL;

extern struct threadpool *pool;

Sending_File_Link sending_file_head = NULL;

int z_start_server(const int port, const char *ip)
{
    int ret = zchat_open_db("/home/jsetc/cs/suqian_chat.db");
    if (ret != SQLITE_OK)
        return -1;

    int sockfd = z_socket();
    if (sockfd == -1)
    {
        return -1;
    }
    if ((sockfd = z_bind(sockfd, port, ip)) == -1)
    {
        return -1;
    }

    printf("wating client to connect server......\n");
    while (1)
    {
        int c_port;
        char c_ip[16] = "\0";
        //等待客户端连接

        int conn = z_accept(sockfd, &c_port, c_ip);
        if (-1 == conn)
            continue;

        printf("accept client port = %d ip = %s connect: %d!\n", c_port, c_ip, conn);

        //连接上服务器
        //start_detach_pthread(NULL, zchat_server_controller, &conn);
        threadpool_add_job(pool,zchat_server_controller,(void *)&conn);
    }
    file_list_destory(sending_file_head);
    destory(client_head);
    //file_list_destory(sending_file_head);
    zchat_close_db();
}

//客户端服务端交换控制层
void * zchat_server_controller(void *arg)
{
    int sockfd = *(int *)(arg);
    struct msg_header header;
    int head_len = sizeof(header);

    while (1)
    {
        int r_len = recv(sockfd, &header, head_len, 0);
  
        if (r_len == 0 || r_len == -1)
        {
            //客户端关闭链接
            printf("client %d cut connection!\n", sockfd);
            break;
        }


        //解析消息头
        if (strncmp((const char *)header.header, "SQJY", 4) == 0)
        {

            //符合协议
            if (verify_header(header) == -1)
                continue;

            //接收正文
            char content[header.content_len];
            r_len = recv(sockfd, content, header.content_len, 0);
            if (r_len == 0 || r_len == -1)
            {
                //客户端关闭连接
                printf("client %d cut connection!\n", sockfd);
                break;
            }

            //传递下一层处理
            process_client_msg(sockfd, content, header.control_mask);
        }
        else
        {
            printf("client %d send error header%s!\n", sockfd, header.header);
            break;
        }
    }
    //用户下线
    delete_node(&client_head, sockfd);
    close(sockfd);

    //给所有其他用户发送在线用户情况
    send_upline_users(-1, 1);

    pthread_exit(NULL);
}

//校验消息头
int verify_header(struct msg_header header)
{
    if (header.content_len == -1)
        return -1;

    if (header.control_mask == -1)
        return -1;

    return 0;
}

void process_client_msg(int sockfd, const char *content, unsigned short control_mask)
{
    switch (control_mask)
    {
    case USER_LOGIN:
        user_login_handle(sockfd, content);
        break;
    case REGISTER:
        register_handle(sockfd, content);
        break;
    case MSG_SEND:
        send_msg_handle(sockfd, content);
        break;
    case FILE_SEND_REQUEST:
        file_send_request_service(sockfd, content);
        break;
    case FILE_SEND_REPLAY:
        file_send_replay(sockfd, content);
        break;
    case FILE_SEND:
        file_send_service(sockfd, content);
        break;
    case SILENCE:
        silence_someone(sockfd,content);
        break;
    case CANCEL_SILENCE:
        cancel_silence_someone(sockfd,content);
        break;
    case KICK:
        kick_someone(sockfd,content);
        break;
    default:
        break;
    }
}