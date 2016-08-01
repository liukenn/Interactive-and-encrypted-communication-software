#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define SERVPORT 3333	/*服务器监听端口号*/
#define MAXDATASIZE 100		/*每次传输最大数据容量*/
int main(int argc,char *argv[])
{
	FILE *ft;/*文件指针*/
	int sockfd/*监听socket*/,recvbytes/*数据传输socket*/,i=0;
	char str_file[5000]/*存储文件的缓存*/,temp/*临时存储缓存*/,str_name[50]/*存储文件名的缓存*/,str_lu[50]/*存储文件路径的缓存*/;
	struct hostent *host;/*本地IP地址*/
	struct sockaddr_in serv_addr;/*服务器 IP地址*/
	//---------------------------
	if(argc<2){/*判断是否有输入服务器ip地址*/
		fprintf(stderr,"Please enter teh server's hostname!\n");
		exit(1);
	}
	//------------gethostbyname---------------
	if((host=gethostbyname(argv[1]))==NULL){/*获取本地IP地址*/
		herror("gethostbyname err!");
		exit(1);
	}
	//--------------socket-------------
	if((sockfd=socket(AF_INET/*主机字节顺序*/,SOCK_STREAM/*链接类型*/,0/*选择默认的协议*/))==-1){/*表示使用TCP协议进行连接*/
		perror("socket err!");
		exit(1);
	}
	//---------------------------
	serv_addr.sin_family=AF_INET;/*主机字节顺序*/
	serv_addr.sin_port=htons(SERVPORT);/*短整型数，网络字节顺序*/
	serv_addr.sin_addr=*((struct in_addr *)host->h_addr);/*获取服务器IP地址*/
	bzero(&(serv_addr.sin_zero),8);/*其余置为0*/
	//-------------connect--------------
	if(connect(sockfd/*监听socket*/,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))==-1){/*与服务器建立连接*/
		perror("connect err!");
		exit(1);
	}
	//---------------------------
	printf("请输入文件名:");
	scanf("%s",str_name);/*用户输入文件名字*/
	ft=fopen(str_name,"rb");/*根据文件名字打开文件*/
	while((temp=fgetc(ft))!=EOF)/*当还没读到文件最后时一直读取文件*/
	{
		str_file[i++]=temp;
	}
	close(ft);/*关闭文件指针*/
	str_file[i]=='\0';/*添加结束标志*/
	//-----------send----wenjian------------
	if(send(sockfd/*监听socket*/,str_file/*文件*/,strlen(str_file),0)==-1){/*将文件发给服务器*/
		perror("send err!");
		exit(1);
	}
	//-----------send------lujing----------
	printf("请输入文件备份路径:");
	scanf("%s",str_lu);/*用户输入文件备份路径*/
	strcat(str_lu,str_name);/*将文件名和文件备份路径链接在一起*/
	if(send(sockfd/*监听socket*/,str_lu/*文件备份路径*/,strlen(str_lu),0)==-1){/*将文件名和文件备份路径一起传给服务器*/
		perror("send err!");
		exit(1);
	}
	printf("备份成功!\n");
	//---------------------------
	close(sockfd);
	return 0;
}
