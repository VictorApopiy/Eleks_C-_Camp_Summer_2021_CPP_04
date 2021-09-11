#pragma once
#include <string>
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <cassert>
#define MESSAGE_SIZE 16
#define BUFFER_SIZE 1024
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)
/*
This class was created just for test of server and will be changed in the future
Basically, information and methods are analogue to server class
*/
class Client {
private:
	WSAData WSA;
	char szMainBuffer[BUFFER_SIZE];
	SOCKET ClientSocket;
	sockaddr_in ClientInfo;
	int iClientInfoSize;
	std::string sIpAddress;
	int iPort;
private:
	void init();
public:
	Client(std::string sIpAddress, int iPort);
	~Client();
	void RecvFile();
	void ConnectToServer();
};
//отче наш...
