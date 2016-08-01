//客户端程序
#include< stdio.h >
#include< stdlib.h >
#include< windows.h >
#include< winsock.h >
#include< string.h > 

#pragma comment( lib, "ws2_32.lib" )

#define PORT 2046
#define BACKLOG 10
#define MAXDATASIZE 100


void main( void )
{
	int iClientSock;
	char buf[ MAXDATASIZE ];
	struct sockaddr_in ServerAddr;
	int numbytes;

	WSADATA WSAData;



	if( WSAStartup( MAKEWORD( 1, 1 ), &WSAData ) )//初始化
	{
		printf( "initializationing error!\n" );
		WSACleanup( );
		exit( 0 );
	}

	if( ( iClientSock = socket( AF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET )
	{
		printf( "创建套接字失败!\n" );
		WSACleanup( );
		exit( 0 );
	}

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons( PORT );
	ServerAddr.sin_addr.s_addr = inet_addr( "127.0.0.1" );//记得换IP
	memset( &( ServerAddr.sin_zero ), 0, sizeof( ServerAddr.sin_zero ) );

	if( connect( iClientSock, ( struct sockaddr * ) & ServerAddr, sizeof( struct sockaddr ) ) == -1 )
	{
		printf( "connect失败!" );
		WSACleanup( );
		exit( 0 );
	}

	numbytes = recv( iClientSock, buf, MAXDATASIZE, 0 );

	if( numbytes == -1 )
	{
		printf( "recv失败!" );
		WSACleanup( );
		exit( 0 );
	}

	buf[ numbytes ] = '\0';

	printf( "Received: %s", buf );

	closesocket( iClientSock );
	WSACleanup( );
}
