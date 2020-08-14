#pragma once
#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")
class NETWORK
{
public:
	NETWORK();
	~NETWORK();
	void initserver();
	void initclient();
	void readdata(char * buffer, __int32 size);
	void senddata(char * buffer, __int32 size);
	void stop();
	SOCKET s, s2;
	bool stopped = false;
};

