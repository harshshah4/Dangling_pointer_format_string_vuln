#define _GNU_SOURCE
#include <sys/wait.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>

#define STACK_SIZE (1024 * 1024)

#define PORT 4444
struct clone_args {
	int newSocket;
	struct sockaddr_in newAddr;
};
void deadCode(char *buff){
	printf("inside deadCode %s and address is %x\n", buff,(unsigned int)buff);
	free(buff);
	sleep(10);
}
char argv_value[1024];
int n =0 ;
int function(void *arg){
	char *buffer = (char *)malloc(1024);
	printf("buffer address after malloc %x\n ", (unsigned int)buffer);
	struct clone_args *args = (struct clone_args*)arg;
	int sock = args->newSocket;
	char ip[15]; strcpy(ip,inet_ntoa(args->newAddr.sin_addr));
	int port = ntohs(args->newAddr.sin_port);
	char deadCode_buff[1024];
	while(1){
		printf("%s","waiting for receive");
		n=recv(sock, buffer, 1024, 0);
		buffer[n]='\0';
		printf("value recieved from remote in buffer : %s\n",buffer);
        if(strcmp(buffer,"dummy") == 0){
           	printf("attacker: %x \n",(unsigned int)buffer);
			strcpy(deadCode_buff,argv_value);
			printf(deadCode_buff);
			puts(buffer);
            send(sock, buffer, strlen(buffer), 0);
			printf("sent data through %d is %s \n", port, buffer);
        }
        else{
          	if(strcmp(buffer, ":exit") == 0){
             	printf("Disconnected from %s:%d breaking now\n", ip, port);
                break;
            }else{
                printf("received data %s is at address %x", buffer, (unsigned int)buffer);
                send(sock, buffer, strlen(buffer), 0);
				printf("sent data through %d is %s \n", port, buffer);
            }
        }
	}
	close(sock);
	return 0;
}
int main(int argc, char *argv[]){

	strcpy(argv_value,argv[1]);
	int sockfd, ret;
	 struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char *stackTop;
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("%s\n","[-]Error in connection.");
		exit(1);
	}
	printf("%s\n","[+]Server Socket is created.");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("0.0.0.0");

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("%s\n","[-]Error in binding.");
		exit(1);
	}
	printf("[+]Bind to port %d\n", 4444);

	if(listen(sockfd, 10) == 0){
		printf("%s\n","[+]Listening....");
	}else{
		printf("%s\n","[-]Error in binding.");
	}


	void *arg;
	struct clone_args args;
	while(1){
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
	        stackTop = malloc(STACK_SIZE) + STACK_SIZE;
		args.newSocket = newSocket;
		args.newAddr = newAddr;
		arg = (void *)&args;	
		childpid = clone(function, stackTop, CLONE_VM|SIGCHLD, (void *)arg);
		if(childpid == -1){
			close(sockfd);
		}
	}
	return 0;
}


