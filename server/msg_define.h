#pragma once

#define REGISTER 0x11
#define USER_LOGIN 0x12
#define LOGIN_REPLAY 0x13
#define REGISTER_REPLAY 0x14

#define UPLINE_USERS 0x22

#define CODE_ERROR 0x44
#define CODE_SUCCESS 0x55

#define FILE_SEND 0x65
#define FILE_SEND_REQUEST 0x66
#define FILE_RECV_REFUSE 0x67
#define FILE_SEND_REPLAY 0x68
#define FILE_RECV_CONFIRM 0x69

#define SUCCESS 1
#define ERROR -1

#define SILENCE 0x77
#define CANCEL_SILENCE 0x79
#define KICK 0x78

#define MSG_SEND 0x33
#define MSG_RECV 0x44

#define HEAD_LEN sizeof(struct mag_header);
//协议定义
/*
协议头+协议正文+正文内容
防止 随便读取1024
*/

//协议头
struct msg_header
{
    //消息的起始标志 SQJY
    unsigned char header[5];

    //具体请求服务端进行的操作
    unsigned short control_mask;

    //正文的长度
    unsigned int content_len;

    //保留位
    unsigned short reservert;

};

//注册正文结构体
struct user_struct
{
    unsigned char user_name[32];

    unsigned char pass_word[32];
};

struct server_replay
{
    unsigned char error_msg[1024];

    unsigned char code;
};

struct online_user
{
    unsigned char user_names[1023][32];

    unsigned short count;
};

struct msg_define
{
    unsigned char user_name[1024];

    unsigned char msg[1024];

    int is_send_all;

};

//文件信息
//消息头+文件信息+真正文件流
struct file_info_content
{
    unsigned char recv_user_name[1024];

    unsigned char send_user_name[1024];

    unsigned char file_name[1024];

    unsigned long file_size;

    unsigned int file_buffer_length;
};

struct file_send_replay
{
    unsigned char recv_user[32];

    unsigned char send_user[32];

    unsigned char error_msg[1024];

    unsigned short code;
};
