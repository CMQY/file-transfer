/*
 * client.c
 *
 *  Created on: 2015年4月11日
 *      Author: opensuse
 */


#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<linux/in.h>
#include<signal.h>
#include<fcntl.h>

static int s;


#define PORT 8888
#define MAX_PATH 256
#define BUFF_LEN 512

/*
 * 文件传输函数
 */
int filetransfer(int conn, int file, char *filename)
{

	char buf[BUFF_LEN];
	int filename_length=0;
	int file_length=0;
	int ret_len=0;
	int ret=0;

	 //获得文件信息
	struct stat stat;
	bzero(&stat,sizeof(struct stat));
	filename_length=strlen(filename);
	fstat(file,&stat);
	file_length=stat.st_size;

	//发送第一阶段信息
	bzero(buf,BUFF_LEN);

	write(conn,(void *)&filename_length,sizeof(int));
	write(conn,filename,filename_length);
	write(conn,&file_length,sizeof(int));

	printf("first part mesg is sended\n");

	//接收确认
	recv(conn,buf,1,MSG_WAITALL);
	printf("recieve a mesg\n");

	//开始发送数据


	ret_len=file_length;
	while(ret_len)
	{
		ret=read(file,buf,BUFF_LEN);
		ret=write(conn,buf,ret);
		if(ret<0)
		{
			perror("seng mesg error");
			return -1;
		}
		ret_len-=ret;
	}

	return 0;

}



/*
 * 客户端连接处理程序
 */
int proccess_conn_client(int fd)
{
	int ret=0;
	char filename[MAX_PATH];
	bzero(filename,MAX_PATH);
	printf("PLS input the file name\n");
	scanf("%s",filename);
	ret=open(filename,O_RDONLY);
	if(ret<0)
	{
		perror("file open error\n");
		exit(-1);
	}

	ret=filetransfer(fd,ret,filename);
	if(ret!=0)
	{
		printf("transfer file error...\n");
		return -1;
	}

	return 0;
}

int main(int argc,char *argv[])
{
  struct sockaddr_in server_addr;	//服务器地址结构
  unsigned long inaddr=1;
  int err;
  struct hostent *host;


  if(argc==1){
    printf("PLS input server addr\n");
    return 0;
  }


  s=socket(AF_INET,SOCK_STREAM,0);
  if(s<0){
    printf("socket error");
    return -1;
  }

  bzero(&server_addr,sizeof(struct sockaddr_in));	//设置服务器地址结构
  server_addr.sin_family=AF_INET;
  server_addr.sin_port=htons(PORT);


  inaddr=inet_addr(argv[1]);
  memcpy((char *)&server_addr.sin_addr,&inaddr,sizeof(inaddr));

  err=connect(s,(struct sockaddr *)&server_addr,sizeof(struct sockaddr));
  if(err<0){
    printf("connect");
    return -1;
  }

  proccess_conn_client(s);	//客户端处理过程

  close(s);//关闭连接

  return 0;
}
