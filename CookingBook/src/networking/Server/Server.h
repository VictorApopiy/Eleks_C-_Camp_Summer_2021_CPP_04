#ifndef Server
#define Server

#include <iostream>
#include <WinSock2.h>
#include <cassert>
#include <fstream>
#include <string>
#include <process.h>
#include <thread>
#include <list>
#include <sstream>
#include <ctime>
#include "E:/EleksApp/Eleks_C_Camp_Summer_2021_CPP_04/CookingBook/src/DataBase/CUserDataBase.h"
#include "E:/EleksApp/Eleks_C_Camp_Summer_2021_CPP_04/CookingBook/src/DataBase/sqlite3.h"

const std::string g_sServerLogFileName = "ServerLogFile.bin";
const std::string sIPAddressFileName = "IPAddressFile.bin";
const size_t g_uBufferSize  = 1024;
const size_t g_iMessageSize = 16;

#pragma warning(disable:4996)

#pragma comment(lib, "ws2_32.lib")
/*
CServer is a class that holds all the essential information about server.
It has methods to initialize and run server
But it also can handle client in seperate thread and send file to it
*/
class CServer {
private:
	WSAData WSA;
	SOCKET ListeningSocket;
	sockaddr_in ServerInfo;//Information about server (port, address, TCP)
	int iInfoSize;//size of this information
	std::string sIpAddress;
	int iPort;
	char szMainBuffer[g_uBufferSize];//main buffer to send and receive info from clients
	std::ofstream ServerLogFile;//log file of server
	std::list<std::thread> Threads;//thread pool for handling multiple clients
	std::fstream IPAddressFile;
	std::time_t Time;
	std::tm* CurrentTime;

	UsersDataBase::CUsersDataBase &refUserDataBase{UsersDataBase::CUsersDataBase::GetUsersDataBase()};
private:
	void Init();
public:
	CServer();
	~CServer();
	void Start();
	//method for sending file to client. it receives string filename and client socket as a parameters
	void SendFile(const std::string& sFileName,SOCKET& ClientSocket);
	void HandleClient(SOCKET ClientSocket);
	//checking the existence of ip address for server in special file
	bool CheckIP();
	//if there is no ip address in file this func will write it to special file and set everything up 
	void SetServer(const std::string& sIpAddress, int iPort);
	void LogInfo(const std::string& sInfo);
	bool TryRegisterUser(SOCKET& ClientSocket);
	bool TryLoginUser(SOCKET&ClientSocket);
};

#endif 