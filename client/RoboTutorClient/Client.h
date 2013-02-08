#pragma once

#define SERVER_PORT 8311 //12345
#define BUF_SIZE 4096 // block transfer size  
#define QUEUE_SIZE 10
#define IPAddress "145.94.188.91" // Local to the system - Loop back address

class Client
{
public:
	WORD		wVersionRequested;
	WSADATA		wsaData;
	SOCKADDR_IN target; //Socket address information
	SOCKET		s;
	int			err;
	int			bytesSent;
	int			bytesRecv;
	char		buf[20];
	char		recvbuf[11]; 

	Client(void);
	~Client(void);
	bool connectToNao();
	bool closeConnection();
	int sendToNao();
	int rcvFromNao();

};

