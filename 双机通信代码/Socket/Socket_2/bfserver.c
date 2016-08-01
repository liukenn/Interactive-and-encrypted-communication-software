#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#define SERVPORT 3333	/*服务器监听端口号*/
#define BACKLOG 10		/*最大同时连接请求数*/
int main()
{
	FILE *ft;/*文件指针*/
	int sockfd/*监听socket*/,recvbytes/*recv接受数据的大小*/,client_fd/*数据传输socket*/,sin_size,i=0;
	char str_file[5000]/*存储文件的缓存*/,str_lu[100]/*存储文件路径的缓存*/;
	struct sockaddr_in my_addr/*本机地址信息*/;
	struct sockaddr_in remote_addr/*客服端地址信息*/;
	//-------------------socket-------------------
	if((sockfd=socket(AF_INET/*主机字节顺序*/,SOCK_STREAM/*链接类型*/,0/*选择默认的协议*/))==-1)/*表示使用TCP协议进行连接*/
	{
		perror("socket err!");
		exit(1);
	}
	//--------------------------------------
	my_addr.sin_family=AF_INET;/*主机字节顺序*/
	my_addr.sin_port=htons(SERVPORT);/*短整型数，网络字节顺序*/
	my_addr.sin_addr.s_addr=INADDR_ANY;/*自动获取本机地址*/
	bzero(&(my_addr.sin_zero),8);/*其余置为0*/
	//-----------------bind---------------------
	if(bind(sockfd/*监听socket*/,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))==-1)
	{
		perror("bind err!");
		exit(1);
	}
	//----------------listen----------------------
	if(listen(sockfd/*监听socket*/,BACKLOG/*最大同时连接请求数*/)==-1)
	{
		perror("listen err!");
		exit(1);
	}
	//--------------------------------------
	while(1)
	{
		//----------------accept----------------------
		sin_size=sizeof(struct sockaddr_in);/*求结构体sockaddr_in大小*/	
		if((client_fd=accept(sockfd/*监听socket*/,(struct sockaddr *)&remote_addr,&sin_size))==-1)/*接受客户端请求*/
		{
			perror("accept err!");
			continue;
		}
		//-----------------recv---wenjian------------------
		if(i==0)
		{
			if((recvbytes=recv(client_fd/*数据传输socket*/,str_file/*存储文件名*/,5000/*最大接受容量*/,0))==-1){/*接受文件*/
				perror("recv file err!");
				exit(1);
			}
			str_file[recvbytes]='\0';/*添加结束标志*/
			i++;
		}
		//-------------------recv-----lujing--------------
		if(i==1)
		{
			if((recvbytes=recv(client_fd/*数据传输socket*/,str_lu/*存储文件路径*/,50/*最大接受容量*/,0))==-1){/*接受路径*/
				perror("recv lu err!");
				exit(1);
			}
			str_lu[recvbytes]='\0';/*添加结束标志*/
			//---------------xieru wenjian-----------------------
			ft=fopen(str_lu,"w");/*根据文件路径打开文件*/
			if(ft==NULL)
			{
				fprintf(stderr,"Can't open!\n");
				exit(1);
			}
			fwrite(str_file/*将要写入的数据*/,1/*写入文件次数*/,strlen(str_file)/*每次写入多大数据*/,ft/*指向文件的文件指针*/);/*写入*/
			fclose(ft);/*关闭指针*/
		}
		close (client_fd);
	//--------------------------------------
	}
	return 0;
}
