#ifndef ___MY_THREAD___
#define ___MY_THREAD___

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <arpa/inet.h>

struct _message
{
	char * file;
	int size;
	int index;
};

void * my_thread(void* a);

#endif
