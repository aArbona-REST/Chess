#include "NETWORK.h"

NETWORK::NETWORK()
{
}
NETWORK::~NETWORK()
{
}

void NETWORK::initserver()
{
	//SOCKET listensocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//sockaddr_in s_add;
	//s_add.sin_family = AF_INET;
	//s_add.sin_port = htons(31337);
	//s_add.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//bind(listensocket, (SOCKADDR*)& s_add, sizeof(s_add));
	//listen(listensocket, 1);
	//sockaddr_in s_add2;
	//s2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//int len = sizeof(s_add2, &len);
	//s2 = accept(listensocket, (SOCKADDR*)& s_add2, &len);
}

void NETWORK::initclient()
{
	//s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//sockaddr_in s_add;
	//s_add.sin_family = AF_INET;
	//s_add.sin_port = htons(31337);
	//s_add.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//int result = connect(s, (SOCKADDR*)& s_add, sizeof(s_add));
	//if (result == SOCKET_ERROR)
	//{
	//	wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
	//	result = closesocket(s);
	//	if (result == SOCKET_ERROR)
	//	{
	//		wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
	//	}
	//	return;//error failed attemp return logic
	//}
	//return;//successfull connection made return logic

}

void NETWORK::readdata(char * buffer, __int32 size)
{
	char c;
	int result = recv(s2, &c, 1, 0);
	if (result < 0)
		return;
	int br = 0;
	while (br < c)
	{
		result = recv(s2, buffer + br, size - br, 0);
		if (result < 0)
			return;
		br += result;
	}
}

void NETWORK::senddata(char * buffer, __int32 size)
{
	if (size > 255)
		return;
	unsigned char i = (unsigned char)size;
	int result = send(s2, (char*)&i, 1, 0);
	if (result < 0)
	{
		if (stopped)
		{
			return;//a shutdown return logic
		}
		return;//a disconnect return logic
	}
	send(s2, buffer, size, 0);
}

void NETWORK::stop()
{
	stopped = true;
	shutdown(s2, 2);
	closesocket(s2);
}
