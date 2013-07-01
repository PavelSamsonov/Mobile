#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include <stdlib.h>

int main(void) {
	struct sockaddr_in addr;
	  int sock,n =0;
	  char recvBuff[1024];
	  const char host[] = "127.0.0.1";

	  sock = socket(AF_INET, SOCK_STREAM, 0);

	  bzero(&addr, sizeof(addr));

	  addr.sin_family = AF_INET;
	  addr.sin_port = htons(3003);
	  inet_aton(host, &addr.sin_addr);

	  if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
	     perror("connect");
	     return 1;
	     }

	  while ( (n = read(sock, recvBuff, sizeof(recvBuff)-1)) > 0) {
		  printf("file received:\n");
	          recvBuff[n] = 0;
			
	          if(fputs(recvBuff, stdout) == EOF) {
	              printf("End of file\n");
	          }
	      }

	      if(n < 0) {
	    	  perror("read");
	    	  return 2;
	      }

	  close(sock);

	  return 0;
}

