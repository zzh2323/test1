#include "client_list.h"

#include <stdlib.h>
#include <stdio.h>

//创建节点
int create_node(Link *newnode)
{
    (*newnode) = (Link)malloc(sizeof(Node));

    if ((*newnode) == NULL)
    {
        printf("create client info node malloc error!\n");
        return -1;
    }
    return 0;
}

//头插入
void insert_list_head(Link *head, Link newnode)
{
    if ((*head) == NULL)
    {
        (*head) = newnode;
        newnode->next = NULL;
    }
    else
    {
        newnode->next = (*head);
        (*head) = newnode;
    }
}

//删除节点
int delete_node(Link *head, int sockfd)
{
    if ((*head) == NULL)
    {
        printf("client list is NULL!\n");
        return -1;
    }

    Link p, q;
    p = q = *head;
    if ((*head)->sockfd == sockfd)
    {
        printf("delete head!\n");
        (*head) = (*head)->next;
        free(p);
        return 0;
    }

    while (p != NULL && p->sockfd != sockfd)
    {
        q = p;
        p = p->next;
    }

    // sockfd肯定存在，移动到最后一个
    if (q->next != NULL)
    {
        q->next = p->next;
        free(p);
        p = NULL;
    }

    return 0;
}

//遍历连表
void dispaly_list(Link head)
{
    Link p = head;
    if (head == NULL)
    {
        printf("LINKLIST Is NULL!\n");
        return;
    }
    while (p != NULL)
    {
        printf("client: ip %s port %s fd:%d\n", p->user_name, p->pass_word, p->sockfd);
        p = p->next;
    }
}

//删除链表
void destory(Link head)
{
    Link p = head;
    if (head == NULL)
        return;
    while (p != NULL)
    {
        free(p);
        p = NULL;
        p = p->next;
    }
}