#include "Client.h"

Client::Client() :
	sIpAddress(""),
	iPort(0),
	ClientSocket(INVALID_SOCKET){
	ZeroMemory(&ClientInfo, sizeof(ClientInfo));
	iClientInfoSize = sizeof(ClientInfo);
}
Client::~Client() {
	WSACleanup();
	closesocket(ClientSocket);
}
void Client::SetClient(const std::string& sIpAddress, const int iPort){
	this->sIpAddress = sIpAddress;
	this->iPort = iPort;
	IPAddressFile.open(sIPAddressFileName,std::ios::binary|std::ios::out|std::ios::trunc);
	IPAddressFile<<sIpAddress<<std::endl;
	IPAddressFile<<iPort;
	IPAddressFile.close();
}
bool Client::CheckIP(){
	IPAddressFile.open(sIPAddressFileName,std::ios::ate|std::ios::binary);
	bool bResult;
	if(IPAddressFile.is_open()){
		if(IPAddressFile.tellg()==0){
			bResult = false;
		}
		else{
			IPAddressFile.close();
			IPAddressFile.open(sIPAddressFileName,std::ios::binary);
			std::getline(IPAddressFile,sIpAddress);
			std::string sPort;
			std::getline(IPAddressFile,sPort);
			iPort = stoi(sPort);
			bResult = true;
		}
		IPAddressFile.close();
	}
	else{
		bResult = false;
	}
	return bResult;
}
bool Client::TryConnection(){
	Init();
	bool bResult ;
	if(connect(ClientSocket, reinterpret_cast<const sockaddr*>(&ClientInfo), iClientInfoSize)==SOCKET_ERROR){
		bResult = false;
	}
	else{
		bResult = true;
	}
	return bResult;
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
	char *Buffer = new char[iRecvBufferSize];
	std::ofstream File;
	File.open(sFileName, std::ios::binary);
	recv(ClientSocket,Buffer,iRecvBufferSize,0);
	File.write(Buffer,iRecvBufferSize);
	delete[] Buffer;
	File.close();
}
void Client::Init() {
	assert(!WSAStartup(MAKEWORD(2, 2), &WSA)&&"Couldn`t WSA on client");
	ClientInfo.sin_addr.S_un.S_addr = inet_addr(sIpAddress.c_str());
	ClientInfo.sin_port = htons(iPort);
	ClientInfo.sin_family = AF_INET;
	assert(!((ClientSocket = socket(AF_INET , SOCK_STREAM, 0))==SOCKET_ERROR)&&"Couldn`t create socket on client");
}
//this function is just for the test of server, the idea is that we will have various functions
// for different requests, so server will handle them
void Client::ConnectToServer() {
	if(TryConnection()){
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
	else{
		std::cout<<"Can`t interact with server"<<std::endl;
	}
} 