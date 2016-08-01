#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define SERVPORT 3335	/*服务器监听端口号*/
#define MAXDATASIZE 100		/*每次传输最大数据容量*/
int main(int argc,char *argv[])
{
	int sockfd/*监听socket*/,recvbytes/*数据传输socket*/;
	char buf[MAXDATASIZE];/*缓存，存储服务器返回的时间*/
	struct hostent *host;/*本地IP地址*/
	struct sockaddr_in serv_addr;/*服务器 IP地址*/
	//---------------------------
	if(argc<2){/*判断是否有输入服务器ip地址*/
		fprintf(stderr,"Please enter teh server's hostname!\n");
		exit(1);
	}
	//----------gethostbyname-----------------
	if((host=gethostbyname(argv[1]))==NULL){/*获取本地IP地址*/
		herror("gethostbyname 创建失败!");
		exit(1);
	}
	//-----------socket----------------
	if((sockfd=socket(AF_INET/*主机字节顺序*/,SOCK_STREAM/*链接类型*/,0/*选择默认的协议*/))==-1){/*表示使用TCP协议进行连接*/
		perror("socket 创建失败!");
		exit(1);
	}
	//---------------------------
	serv_addr.sin_family=AF_INET;/*主机字节顺序*/
	serv_addr.sin_port=htons(SERVPORT);/*短整型数，网络字节顺序*/
	serv_addr.sin_addr=*((struct in_addr *)host->h_addr);/*获取服务器IP地址*/
	bzero(&(serv_addr.sin_zero),8);/*其余置为0*/
	//-------------connect--------------
	if(connect(sockfd/*监听socket*/,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))==-1){/*与服务器建立连接*/
		perror("connect 创建失败!");
		exit(1);
	}
	//-----------recv----------------
	if((recvbytes=recv(sockfd/*监听socket*/,buf/*存储时间*/,MAXDATASIZE/*每次传输最大数据容量*/,0))==-1){/*接收服务器的响应*/
		perror("recvbytes 创建失败!");
		exit(1);
	}
	buf[recvbytes]='\0';/*最后添加结束标志*/
	printf("\n时间:%s\n",buf);/*打印时间*/
	close(sockfd);/*关闭*/
	return 0;
}
