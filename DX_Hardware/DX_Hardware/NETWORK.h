#pragma once
#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")
class NETWORK
{
public:
	NETWORK();
	~NETWORK();
	void NETWORK::initserver();
	void NETWORK::initclient();
	void NETWORK::readdata(char * buffer, __int32 size);
	void NETWORK::senddata(char * buffer, __int32 size);
	void NETWORK::stop();
	SOCKET s, s2;
	bool stopped = false;
};

