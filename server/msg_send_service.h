#pragma once
#include "msg_define.h"
#include "client_list.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include "common_utils.h"
#include "register_service.h"
#include "z_block_server.h"

void send_msg_handle(int sockfd, const char *content);

void silence_someone(int sockfd, const char *content);

void cancel_silence_someone(int sockfd, const char *content);

void kick_someone(int sockfd, const char *content);