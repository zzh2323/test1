#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

struct job
{
    void *(*func)(void *arg);
    void *arg;
    struct job *next;
};

struct threadpool
{
    int thread_num; //已开启的线程数量

    pthread_t *pthread_ids; //保存线程池中线程的id

    struct job *head; //任务队列的头

    struct job *tail; //任务队列的尾

    int queue_max_num; //任务队列的最大数

    int queue_cur_num; //任务队列已有多少任务

    pthread_mutex_t mutex;

    pthread_cond_t queue_empty; //任务队列为空的条件

    pthread_cond_t queue_not_empty; //任务队列不为空的条件

    pthread_cond_t queue_not_full; //任务队列不为满的条件
};

void *threadpool_function(void *arg);

struct threadpool *threadpool_init(int thread_num, int queue_max_num);

void threadpool_add_job(struct threadpool *pool, void *(*func)(void *), void *arg);

void thread_destory(struct threadpool *pool);