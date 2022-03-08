
#include "z_block_server.h"
#include "zchat_server_controller.h"
#include "thread_pool.h"
#include <signal.h>

struct threadpool *pool = NULL;

void handle_pipe(int sig)
{
    // do nothing
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("please input port ip to start server!\n");
        return -1;
    }
    pool = threadpool_init(10,100);


    hadle_pie_main();

    int port = atoi(argv[1]);
    const char *ip = argv[2];
    printf("chat server start with port = %d ip = %s\n", port, ip);

    //启动服务端
    z_start_server(port, ip);

    return 0;
}

//处理客户端PIPE,防止客户端异常退出引发的PIPE异常
void hadle_pie_main()
{
    struct sigaction sa;
    sa.sa_handler = handle_pipe;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGPIPE, &sa, NULL);
}
