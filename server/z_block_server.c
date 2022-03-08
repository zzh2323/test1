#include "z_block_server.h"

int z_socket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("SOCKET ERROR!\n");
        return -1;
    }

    return sockfd;
}

int z_bind(int sockfd, const int port, const char *ip)
{
    if (port <= 0 || ip == NULL || sockfd <= 0)
    {
        return -1;
    }

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(port);
    server_sockaddr.sin_addr.s_addr = inet_addr(ip);

    struct sockaddr_in client_sockaddr;
    int length = sizeof(client_sockaddr);

    int opt = 1;
    //设置端口复用

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(sockfd, (struct sockaddr *)&server_sockaddr, sizeof(server_sockaddr)) == -1)
    {
        perror("BIND ERROR!\n");
        return -1;
    }

    listen(sockfd, 36);

    return sockfd;
}

int z_accept(int sockfd, int *client_port, char *client_ip)
{
    struct sockaddr_in client_sockaddr;
    socklen_t length = sizeof(client_sockaddr);
    bzero(&client_sockaddr,length);

    int conn = accept(sockfd, (struct sockaddr *)&client_sockaddr, &length);

    if (-1 == conn)
    {
        perror("ACCEPT ERROR!\n");
        return -1;
    }

    *client_port = ntohs(client_sockaddr.sin_port);

    strcpy(client_ip, inet_ntoa(client_sockaddr.sin_addr));

    return conn;
}