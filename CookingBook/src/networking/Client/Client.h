#ifndef CClient
#define CClient

#include <string>
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <cassert>
#include <ctime>
const int g_iMessageSize  = 16;
const int g_iBufferSize  = 1024;
const std::string sIPAddressFileName = "IPAddressFile.bin";
const std::string sClientLogFileName = "ClientLogFile.bin";
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)
/*
This class was created just for test of server and will be changed in the future
Basically, information and methods are analogue to server class
*/
class Client {
private:
	WSAData WSA;
	char szMainBuffer[g_iBufferSize];
	SOCKET ClientSocket;
	sockaddr_in ClientInfo;
	int iClientInfoSize;
	std::string sIpAddress;
	int iPort;
	std::fstream IPAddressFile;
	std::fstream ClientLogFile;
	std::time_t Time;
	std::tm* CurrentTime;
private:
	void Init();
public:
	Client();
	~Client();
	void RecvFile();
	void ConnectToServer();
	bool CheckIP();
	void SetClient(const std::string& sIpaddress,const int iPort);
	bool TryConnection();
	void LogInfo(const std::string& sInfo);
	
};
#endif //Client
//отче наш...