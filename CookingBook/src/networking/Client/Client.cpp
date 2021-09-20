#include "Client.h"
Client::Client(const std::string& sIpAddress,const int iPort) :
	sIpAddress(sIpAddress),
	iPort(iPort),
	ClientSocket(INVALID_SOCKET){
	ZeroMemory(&ClientInfo, sizeof(ClientInfo));
	iClientInfoSize = sizeof(ClientInfo);
}
Client::~Client() {
	WSACleanup();
	closesocket(ClientSocket);
}
void Client::RecvFile(){
	std::cout << "Enter file name" << std::endl;
	std::cin >> szMainBuffer;
	send(ClientSocket, szMainBuffer,strlen(szMainBuffer)+1, 0);
	const std::string sFileName(szMainBuffer);
	char szTempSize[g_iMessageSize];
	recv(ClientSocket, szTempSize, g_iMessageSize, 0);
	if (strstr(szTempSize, "ERROR")) {
		std::cout << "Couldn`t open file on server" << std::endl;
		return;
	}
	int iRecvBufferSize = atoi(szTempSize);
	std::ofstream File;
	File.open(sFileName, std::ios::binary);
	while(iRecvBufferSize>0){
		recv(ClientSocket,szMainBuffer,g_iBufferSize,0);
		File.write(szMainBuffer,g_iBufferSize);
		iRecvBufferSize-=g_iBufferSize;
	}
	File.close();
}
void Client::Init() {
	assert(!WSAStartup(MAKEWORD(2, 2), &WSA)&&"Couldn`t WSA on client");
	ClientInfo.sin_addr.S_un.S_addr = inet_addr(sIpAddress.c_str());
	ClientInfo.sin_port = htons(iPort);
	ClientInfo.sin_family = AF_INET;
	assert(!((ClientSocket = socket(AF_INET , SOCK_STREAM, 0))==SOCKET_ERROR)&&"Couldn`t create socket on client");
}
void Client::ConnectToServer() {
	Init();
	connect(ClientSocket, reinterpret_cast<const sockaddr*>(&ClientInfo), iClientInfoSize);
	while (true) {
		std::cout << "Enter message" << std::endl;
		std::cin >> szMainBuffer;
		send(ClientSocket, szMainBuffer, strlen(szMainBuffer)+1, 0);
		if (strstr(szMainBuffer, "file")) {
			RecvFile();
			ZeroMemory(szMainBuffer, sizeof(szMainBuffer));
		}
		else {
			recv(ClientSocket, szMainBuffer, g_iBufferSize, 0);
			std::cout << "Message received:" << std::endl;
			std::cout << szMainBuffer << std::endl;
		}
	}
}