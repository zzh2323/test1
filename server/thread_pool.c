#include "thread_pool.h"

void *threadpool_function(void *arg)
{
    struct threadpool *pool = (struct threadpool *)arg;
    struct job *pjob = NULL;

    while (1)
    {
        pthread_mutex_lock(&(pool->mutex));

        while (pool->queue_cur_num == 0)
        {
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->mutex));
        }

        pjob = pool->head;
        pool->queue_cur_num--;
        if(pool->queue_cur_num != pool->queue_max_num)
        {
            pthread_cond_broadcast(&(pool->queue_not_full));
        }
        

        if (pool->queue_cur_num == 0)
        {
            pool->head = pool->tail = NULL;
        }
        else
        {
            pool->head = pool->tail->next;
        }
        pthread_mutex_unlock(&(pool->mutex));

        pjob->func(pjob->arg);
        free(pjob);
        pjob = NULL;
    }
}

struct threadpool *threadpool_init(int thread_num, int queue_max_num)
{
    struct threadpool *pool = (struct threadpool *)malloc(sizeof(struct threadpool));
    // malloc

    pool->queue_max_num = queue_max_num;
    pool->queue_cur_num = 0;
    pool->head = NULL;
    pool->tail = NULL;

    pthread_mutex_init(&(pool->mutex), NULL);
    pthread_cond_init((&pool->queue_empty), NULL);
    pthread_cond_init((&pool->queue_not_empty), NULL);
    pthread_cond_init((&pool->queue_not_full), NULL);

    pool->thread_num = thread_num;
    pool->pthread_ids = (pthread_t *)malloc(sizeof(pthread_t *) * thread_num);
    // malloc

    for (int i = 0; i < pool->thread_num; i++)
    {
        pthread_create(&(pool->pthread_ids[i]), NULL, threadpool_function, (void *)pool);
    }
    return pool;
}

void threadpool_add_job(struct threadpool *pool, void *(*func)(void *), void *arg)
{
    pthread_mutex_lock(&(pool->mutex));
    while (pool->queue_cur_num == pool->queue_max_num)
    {
        pthread_cond_wait(&(pool->queue_not_full), &(pool->mutex));
    }
    struct job *pjob = (struct job *)malloc(sizeof(struct job));
    // malloc

    pjob->func = func;
    pjob->arg = arg;

    if (pool->head == NULL)
    {
        pool->head = pool->tail = pjob;
        pthread_cond_broadcast(&(pool->queue_not_empty));
    }
    else
    {
        pool->tail->next = pjob;
        pool->tail = pjob;
    }

    pool->queue_cur_num++;
    pthread_mutex_unlock(&(pool->mutex));
}

void thread_destory(struct threadpool *pool)
{
    pthread_mutex_lock(&(pool->mutex));

    while(pool->queue_cur_num != 0)
    {
        pthread_cond_wait(&(pool->queue_empty),&(pool->mutex));
    }
    pthread_mutex_unlock(&(pool->mutex));
    pthread_cond_broadcast(&(pool->queue_not_empty));
    pthread_cond_broadcast(&(pool->queue_not_full));

    for(int i = 0;i < pool->thread_num;i++)
    {
        printf("thread exit!\n");
        pthread_join(pool->pthread_ids[i],NULL);
    }
    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destroy(&(pool->queue_empty));
    pthread_cond_destroy(&(pool->queue_not_empty));
    pthread_cond_destroy(&(pool->queue_not_full));

    free(pool->pthread_ids);

    struct job *tmp;
    while (pool->head != NULL)
    {
        tmp = pool->head;
        pool->head = tmp->next;
        free(tmp);
    }
    free(pool);
    
}
