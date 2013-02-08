#include "stdafx.h"


Client::Client(void)
{

	//--- INITIALIZATION -----------------------------------
	wVersionRequested = MAKEWORD( 1, 1 );
	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) {
		printf("WSAStartup error %ld", WSAGetLastError() );
		WSACleanup();
		return;
	}
	//------------------------------------------------------
	
	//---- Build address structure to bind to socket.--------  
	target.sin_family = AF_INET; // address family Internet
	target.sin_port = htons (SERVER_PORT); //Port to connect on
	target.sin_addr.s_addr = inet_addr (IPAddress); //Target IP
	//--------------------------------------------------------

	
	// ---- create SOCKET--------------------------------------
	s = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP); //Create socket
	if (s == INVALID_SOCKET)
	{
		printf("socket error %ld" , WSAGetLastError() );
		WSACleanup();
		return;
	}  
	//---------------------------------------------------------

	std::cout << "Socket created." << std::endl;
}


Client::~Client(void)
{
}

bool Client::connectToNao()
{
	//---- try CONNECT -----------------------------------------
	if (connect(s, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
	{
		printf("connect error %ld", WSAGetLastError() );
		WSACleanup();
		return false;
	}
	//-----------------------------------------------------------
	
	std::cout << "Connected." << std::endl;
	return true;
}

int Client::sendToNao()
{
	//---- SEND bytes -------------------------------------------
	std::string input("Ding Dong!\n");
	
	std::cout << "Sending..." << std::endl;
	bytesSent = send( s, input.c_str(), input.length(), 0 ); 
	printf( "Bytes Sent: %ld \n", bytesSent );
	return bytesSent;
}

int Client::rcvFromNao()
{
	bytesRecv = recv( s, recvbuf, 11, 0 );
	err = WSAGetLastError( );// 10057 = A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) 
	if ( bytesRecv == 0 || bytesRecv == WSAECONNRESET ) {
      printf( "Connection Closed.\n");
	  WSACleanup();
    }
    printf( " Bytes Recv: %s \n ", recvbuf );
	return bytesRecv;
}

bool Client::closeConnection()
{
	closesocket(s);
	WSACleanup();
	return true;
}