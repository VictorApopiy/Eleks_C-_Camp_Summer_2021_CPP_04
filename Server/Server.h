#include <iostream>
#include <WinSock2.h>
#include <cassert>
#include <fstream>
#include <string>
#include <process.h>
#include <thread>
#include <list>
#include <mutex>
#define SERVER_LOG_FILE_NAME "ServerLogFile.bin"
#pragma warning(disable:4996)
#define BUFFER_SIZE 1024
#define MESSAGE_SIZE 16
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
	char szMainBuffer[BUFFER_SIZE];//main buffer to send and receive info from clients
	std::ofstream ServerLogFile;//log file of server
	std::list<std::thread> Threads;//thread pool for handling multiple clients
private:
	void init();
public:
	CServer(std::string sIpAddress, int iPort);
	~CServer();
	void Start();
	//method for sending file to client. it receives string filename and client socket as a parameters
	void SendFile(std::string& sFileName,SOCKET& ClientSocket);
	void HandleClient(SOCKET ClientSocket);
};
