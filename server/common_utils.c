#include "common_utils.h"
#include "msg_define.h"
#include <string.h>

int  start_detach_pthread(pthread_t * thread,void *start_function,void * arg)
{
    pthread_attr_t attr;
    pthread_t thread_t;
    int ret = 0;

    if(thread == NULL)
    {
        thread = &thread_t;
    }

    //初始化线程属性
    pthread_attr_init(&attr);

    //设置线程的detach属性
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

    ret = pthread_create(thread,&attr,(void *(*)(void*))start_function,arg);
    if(ret < 0)
    {
        printf("pthread create error!\n");
        goto err;
    }

    pthread_detach(thread_t);

err:
    pthread_attr_destroy(&attr);
    return 1;
}


struct msg_header get_init_header(unsigned short msk,int len)
{
    struct msg_header header;
    memset(&header,0,sizeof(header));
    strcpy(header.header,"SQJY");
    header.control_mask = msk;   
    header.content_len = len;
    return header;
}