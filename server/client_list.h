#pragma once
#include <stdio.h>
#include <sqlite3.h>

struct client_info
{
    unsigned char user_name[32];

    unsigned char pass_word[32];
    int sockfd;

    struct client_info *next;
};

typedef struct client_info Node;
typedef struct client_info *Link;

//创建节点
int create_node(Link *newnode);

//头插入
void insert_list_head(Link *head, Link newnode);

//删除节点
int delete_node(Link *head, int sockfd);

//遍历连表
void display_list(Link head);

//删除链表
void destory(Link head);
