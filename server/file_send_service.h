#include <stdio.h>
#include "client_list.h"
#include "msg_define.h"
#include <string.h>
#include "common_utils.h"
#include <sys/socket.h>
#include "sending_file_list.h"


int file_send_request_service(int sockfd, const char *content);

int file_send_replay(int sockfd,const char * content);

int file_send_service(int sockfd, const char *content);