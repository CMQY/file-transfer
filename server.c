/*
 * server.c
 * 采用通用C/S框架，连接后调用处理函数start_connet，在其中调用file_transfer处理文件传输
 *
 *  Created on: 2015年4月11日
 *      Author: opensuse
 */
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<linux/in.h>
#include<string.h>

#define BACKLOG 1
#define PORT 8888
#define MAX_PATH 256

char file_path[MAX_PATH];

extern int file_transfer(int fd, char *dirname);
struct client_info{
	int fd;
	struct sockaddr_in *sockaddr;
};


//连接后调用
void start_connet(struct client_info *client_info)
{
	int ret=0;
	int fd=client_info->fd;
//	printf("some one connect in...\nhis ip is %s",inet_ntoa(client_info->sockaddr->sin_addr));
	ret=file_transfer(client_info->fd,file_path);
	if(ret<0)
	{
		printf("file receive error. exit\n");
		exit(-1);
	}

	printf("transfer successfully... \n");



}


int main(int argc, char* argv[])
{


	int fd_server,fd_client;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int ret;
	int sock_len=sizeof(struct sockaddr_in);

	if(argc<2)
	{
		printf("PLS input pathname...\n");
		return 0;
	}

	strcpy(file_path,argv[1]);
	fd_server=socket(AF_INET,SOCK_STREAM,0);
	if(fd_server<0)
	{
		perror("socket");
		return -1;
	}
	bzero(&server_addr,sizeof(struct sockaddr_in));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(PORT);
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);

	ret=bind(fd_server,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
	if(ret<0)
	{
		perror("bind");
		return -1;
	}

	ret=listen(fd_server,BACKLOG);
	if(ret<0)
	{
		perror("listen");
		return -1;
	}

	printf("start listening ...\n");

		fd_client=accept(fd_server,NULL,0);
		if(fd_client<0)
		{
			perror("accept");
			return -1;
		}


		struct client_info client_info;
		client_info.fd=fd_client;
		client_info.sockaddr=&client_addr;

		start_connet(&client_info);

		close(fd_client);

		return 0;

}

