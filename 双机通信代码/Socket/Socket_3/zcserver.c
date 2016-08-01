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
	FILE *ft;
	int sockfd,recvbytes,client_fd,sin_size,i=0,len=0;
	char str_file[5000],str_lu[100],ch[2];
	//---------
	char user_name[20],user_password[20],temp[20];
	//---------
	struct sockaddr_in my_addr;
	struct sockaddr_in remote_addr;
	//-----------------socket---------------------
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)/*表示使用TCP协议进行连接*/
	{
		perror("socket err!");
		exit(1);
	}
	//--------------------------------------
	my_addr.sin_family=AF_INET;/*主机字节顺序*/
	my_addr.sin_port=htons(SERVPORT);/*短整型数，网络字节顺序*/
	my_addr.sin_addr.s_addr=INADDR_ANY;/*自动获取本机地址*/
	bzero(&(my_addr.sin_zero),8);/*其余置为0*/
	//-------------------bind-------------------
	if(bind(sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))==-1)
	{
		perror("bind err!");
		exit(1);
	}
	//----------------listen----------------------
	if(listen(sockfd,BACKLOG)==-1)
	{
		perror("listen err!");
		exit(1);
	}
	//--------------------------------------
	while(1)
	{
		//----------------accept----------------------
		i=0;
		sin_size=sizeof(struct sockaddr_in);
		if((client_fd=accept(sockfd,(struct sockaddr *)&remote_addr,&sin_size))==-1)
		{
			perror("accept err!");
			continue;
		}
		//----------------recv----------------------
		if((recvbytes=recv(client_fd,ch,2,0))==-1){
			perror("recv file err!");
			exit(1);
		}
		ch[recvbytes]='\0';
		//----------------recv----------------------
		if((recvbytes=recv(client_fd,user_name,20,0))==-1){
			perror("recv file err!");
			exit(1);
		}
		user_name[recvbytes]='\0';
		//---------------recv-----------------------
		if((recvbytes=recv(client_fd,user_password,20,0))==-1){
			perror("recv file err!");
			exit(1);
		}
		user_password[recvbytes]='\0';
		//--------------------------------------
		if(ch[0]=='r')
		{
			ft=fopen("user.txt","a+");
			if(ft==NULL)
			{
				fprintf(stderr,"Can't open!\n");
				exit(1);
			}
			fprintf(ft,"%s\n",user_name);
			fprintf(ft,"%s\n",user_password);
			fclose(ft);
		}
		//--------------------------------------
		if(ch[0]=='l')
		{
			ft=fopen("user.txt","r");
			while(!feof(ft))
			{
				fgets(temp,20,ft);
				len=strlen(temp);
				temp[len-1]='\0';
				if(strcmp(user_name,temp)==0)
				{
					fgets(temp,20,ft);
					len=strlen(temp);
					temp[len-1]='\0';
					if(strcmp(user_password,temp)==0)
					{
						ch[0]='y';
						len=-1;
						if(send(client_fd,ch,2,0)==-1)
						{
							perror("send err!");
							exit(1);
						}
					}
				}
			}
			//----------------send----------------------
			if(len!=-1)
			{
				ch[0]='n';
				if(send(client_fd,ch,2,0)==-1)
				{
					perror("send err!");
					exit(1);
				}
			}
		}
		//----------------recv----------------------
		if(i==0)
		{
			if((recvbytes=recv(client_fd,str_file,5000,0))==-1){
				perror("recv file err!");
				exit(1);
			}
			str_file[recvbytes]='\0';
			i++;
		}
		//-----------------recv---------------------
		if(i==1)
		{
			if((recvbytes=recv(client_fd,str_lu,50,0))==-1){
				perror("recv lu err!");
				exit(1);
			}
			str_lu[recvbytes]='\0';
			ft=fopen(str_lu,"w");
			if(ft==NULL)
			{
				fprintf(stderr,"Can't open!\n");
				exit(1);
			}
			fwrite(str_file,1,strlen(str_file),ft);
			fclose(ft);
		}
		close (client_fd);
	}
	//--------------------------------------
	return 0;
}
