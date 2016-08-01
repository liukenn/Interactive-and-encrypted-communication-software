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
	FILE *ft;
	int sockfd,recvbytes,i=0,len;
	char str_file[5000],temp,str_name[50],str_lu[50];
	//---------
	char user_name[20],user_password[20],ch[2];
	//---------
	struct hostent *host;
	struct sockaddr_in serv_addr;
	//--------------------------------------
	if(argc<2){/*判断是否有输入服务器ip地址*/
		fprintf(stderr,"Please enter teh server's hostname!\n");
		exit(1);
	}
	//-----------------gethostbyname---------------------
	if((host=gethostbyname(argv[1]))==NULL){/*获取本地IP地址*/
		herror("gethostbyname err!");
		exit(1);
	}
	//-----------------socket---------------------
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1){/*表示使用TCP协议进行连接*/
		perror("socket err!");
		exit(1);
	}
	//--------------------------------------
	serv_addr.sin_family=AF_INET;/*主机字节顺序*/
	serv_addr.sin_port=htons(SERVPORT);/*短整型数，网络字节顺序*/
	serv_addr.sin_addr=*((struct in_addr *)host->h_addr);/*获取服务器IP地址*/
	bzero(&(serv_addr.sin_zero),8);/*其余置为0*/
	//------------------connect--------------------
	if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))==-1){
		perror("connect err!");
		exit(1);
	}
	//-------------------------------------------
	printf("注册请输入r /登录请输入l:");
	scanf("%s",ch);
	if(send(sockfd,ch,strlen(ch),0)==-1){
		perror("send err!");
		exit(1);
	}
	//--------------------------------------
	printf("请输入user_name:");
	memset(user_name,0,20);
	scanf("%s",user_name);
	if(send(sockfd,user_name,strlen(user_name),0)==-1){/*发送用户名字*/
		perror("send err!");
		exit(1);
	}
	printf("请输入user_password:");
	memset(user_password,0,20);
	scanf("%s",user_password);
	if(send(sockfd,user_password,strlen(user_password),0)==-1){/*发送用户密码*/
		perror("send err!");
		exit(1);
	}
	//--------------------------------------
	if(ch[0]=='l')
	{
		if((recvbytes=recv(sockfd,ch,2,0))==-1){
			perror("recv file err!");
			exit(1);
		}
		if (ch[0]=='n')
		{
			printf("登录失败！\n");
			exit(1);
		}
		if (ch[0]=='y')
		{
			printf("登录成功！\n");
		}
	}
	//-------------------------------------------
	if(ch[0]=='r')
		printf("注册成功!\n");
	//--------------------------------------
	printf("请输入即将备份的文件名:");
	memset(str_name,0,50);
	scanf("%s",str_name);
	ft=fopen(str_name,"rb");
	while((temp=fgetc(ft))!=EOF)
	{
		str_file[i++]=temp;
	}
	fclose(ft);
	str_file[i]=='\0';
	//-------------------------------------------
	if(send(sockfd,str_file,strlen(str_file),0)==-1){/*发送备份文件给服务器*/
		perror("send err!");
		exit(1);
	}
	//--------------------------------------
	printf("请输入备份文件的存储路径:");
	memset(str_lu,0,50);
	scanf("%s",str_lu);
	strcat(str_lu,str_name);
	if(send(sockfd,str_lu,strlen(str_lu),0)==-1){/*发送备份路径给服务器*/
		perror("send err!");
		exit(1);
	}
	printf("备份成功!\n");
//-------------------------------------------
	close(sockfd);
	return 0;
}
