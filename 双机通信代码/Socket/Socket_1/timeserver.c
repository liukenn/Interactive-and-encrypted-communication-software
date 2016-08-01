#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <time.h>
#define SERVPORT 3335	/*服务器监听端口号*/
#define BACKLOG 10		/*最大同时连接请求数*/
int main()
{
	FILE *ft;/*文件指针*/
	time_t nowtime;/*存储本地时间*/
	int sockfd/*监听socket*/,client_fd/*数据传输socket*/,sin_size/*存储结构体sockaddr_in大小*/;
	const char *str;/*存储客服端IP地址*/
	struct sockaddr_in my_addr/*本机地址信息*/;
	struct sockaddr_in remote_addr/*客服端地址信息*/;
//----------------socket----------------------
	if((sockfd=socket(AF_INET/*主机字节顺序*/,SOCK_STREAM/*链接类型*/,0/*选择默认的协议*/))==-1)/*表示使用TCP协议进行连接*/
	{
		perror("socket 创建失败!");
		exit(1);
	}
//--------------------------------------
	my_addr.sin_family=AF_INET;/*主机字节顺序*/
	my_addr.sin_port=htons(SERVPORT);/*短整型数，网络字节顺序*/
	my_addr.sin_addr.s_addr=INADDR_ANY;/*自动获取本机地址*/
	bzero(&(my_addr.sin_zero),8);/*其余置为0*/
//-----------------bind---------------------
	if(bind(sockfd/*监听socket*/,(struct sockaddr *)&my_addr/*本机地址信息*/,sizeof(struct sockaddr))==-1)
	{
		perror("bind 创建失败!");
		exit(1);
	}
//---------------listen-----------------------
	if(listen(sockfd/*监听socket*/,BACKLOG/*最大同时连接请求数*/)==-1)/*建立监听*/
	{
		perror("listen 创建失败!");
		exit(1);
	}
//--------------------------------------
	ft=fopen("time.txt","a+");/*打开文件*/
	fprintf(ft,"  IP地址		请求时间\n");/*写入文件*/
	fclose(ft);/*关闭文件*/
//--------------------------------------
	while(1)
	{
		ft=fopen("time.txt","a+");/*打开文件*/
		sin_size=sizeof(struct sockaddr_in);/*求结构体sockaddr_in大小*/		
		//----------------accept----------------------
		if((client_fd=accept(sockfd/*监听socket*/,(struct sockaddr *)&remote_addr/*客服端地址信息*/,&sin_size))==-1)/*接受客户端请求*/
		{
			perror("accept 创建失败!");
			continue;
		}
		//--------------------------------------
		str=inet_ntoa(remote_addr.sin_addr/*客服端IP地址*/);/*将客服端IP地址转换成char类型*/
		//--------------------------------------
		if(ft==NULL)
		{
			fprintf(stderr,"Can't open the file!\n");
			exit(1);
		}
		time(&nowtime);/*获取本地时间*/
		fprintf(ft,"%s  ",str);/*将请求IP地址写入文件中*/
		fprintf(ft,"%s\n",ctime(&nowtime));/*将本地时间写入文件中*/
		fclose(ft);/*关闭文件*/
		//--------------------------------------
		printf("\nIP:%s  time:%s \n",str,ctime(&nowtime));/*在服务器中显示请求IP和请求时间*/
		//----------------send------------------
		if(send(client_fd/*数据传输socket*/,ctime(&nowtime)/*时间*/,strlen(ctime(&nowtime))/*时间长度*/,0)==-1){/*发送请求时间给客户端*/
			perror("send 创建失败!");
			exit(1);
		}
		close (client_fd);/*关闭数据传输socket*/
		//--------------------------------------
	}
	return 0;
}
