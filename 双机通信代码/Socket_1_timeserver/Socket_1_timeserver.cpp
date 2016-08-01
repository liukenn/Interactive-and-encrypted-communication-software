//服务器端程序
#include< stdio.h >
#include< stdlib.h >
#include< windows.h >
#include< winsock.h >
#include< string.h > 

#pragma comment( lib, "ws2_32.lib" )

#define PORT 2046
#define BACKLOG 10

void main( void )
{
	int iServerSock;
	int iClientSock;

	char *buf = "hello, world!\n";

	struct sockaddr_in ServerAddr;
	struct sockaddr_in ClientAddr;

	int sin_size;

	WSADATA WSAData;

	if( WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )//初始化
	{
		printf( "initializationing error!\n" );
		WSACleanup( );
		exit( 0 );
	}

	if( ( iServerSock = socket( AF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET )
	{
		printf( "创建套接字失败!\n" );
		WSACleanup( );
		exit( 0 );
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons( PORT );//监视的端口号
	ServerAddr.sin_addr.s_addr = INADDR_ANY;//本地IP
	memset( & ( ServerAddr.sin_zero ), 0, sizeof( ServerAddr.sin_zero ) );


	if( bind( iServerSock, ( struct sockaddr * )&ServerAddr, sizeof( struct sockaddr ) ) == -1 )
	{
		printf( "bind调用失败!\n" );
		WSACleanup( );
		exit( 0 );
	}

	if( listen( iServerSock, BACKLOG ) == -1 )
	{
		printf( "listen调用失败!\n" );
		WSACleanup( );
		exit( 0 );
	}

	while( 1 )
	{
		sin_size = sizeof( struct sockaddr_in );
		iClientSock = accept( iServerSock, ( struct sockaddr * )&ClientAddr, &sin_size );

		if( iClientSock == -1 )
		{
			printf( "accept调用失败!\n" );
			WSACleanup( );
			exit( 0 );
		}

		printf( "服务器连接到%s\n", inet_ntoa( ClientAddr.sin_addr ) );
		if( send( iClientSock, buf, strlen( buf ), 0 ) == -1 )
		{
			printf( "send调用失败!" );
			closesocket( iClientSock );
			WSACleanup( );
			exit( 0 );
		}
	}
}
