#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <resolv.h>

#include <pthread.h>
#include "my_thread.h"
#include "file_reader.h"

#define N 5
int main()
{
  int sock,i;
  struct sockaddr_in addr;
  struct file_info file_to_send = read_file("letter.txt");

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    }

  bzero(&addr, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_port = htons(3003);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) !=0) {
    perror("bind");
    return 1;
    }

  if (listen(sock, 20) != 0) {
    perror("listen");
    return 2;
    }

  
  for (i=0;i<N;i+=1)
  	{
		listen(sock, 10);
		struct _message mes;
			mes.file = file_to_send.content;
			mes.size = file_to_send.size;
  		mes.index = accept(sock, NULL, NULL);
  		pthread_t thread;
  		pthread_create(&thread, NULL, my_thread, (void*) &mes);
  	}

 close(sock);

 return 0;
}

