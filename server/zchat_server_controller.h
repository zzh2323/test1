#pragma once
#include "z_block_server.h"
#include "client_list.h"
#include "msg_define.h"
#include "file_send_service.h"
#include "sending_file_list.h"
#include "thread_pool.h"



//启动服务
int z_start_server(const int port,const char *ip);

void *zchat_server_controller(void *arg);

int verify_header(struct msg_header header);

void process_client_msg(int sockfd,const char * content,unsigned short control_mask);
