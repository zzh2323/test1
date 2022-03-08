#pragma once
#include <pthread.h>
#include <error.h>
#include <stdio.h>


int  start_detach_pthread(pthread_t * thread,void *start_function,void * arg);

struct msg_header get_init_header(unsigned short msk,int len);