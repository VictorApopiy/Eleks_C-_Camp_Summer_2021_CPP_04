#include "Server.h"
CServer::CServer(std::string sIpAddress, int iPort) :
	ListeningSocket(INVALID_SOCKET),
	sIpAddress(std::move(sIpAddress)),
	iPort(iPort) 
{
	ZeroMemory(&ServerInfo, sizeof(ServerInfo));
	iInfoSize = sizeof(ServerInfo);
	ServerLogFile.open(SERVER_LOG_FILE_NAME,std::ios::binary|std::ios::app);
}
CServer::~CServer()
{
	WSACleanup();
	closesocket(ListeningSocket);
	if(ServerLogFile.is_open()){
		ServerLogFile.close();
	}
}
void CServer::init() {
	assert(!WSAStartup(MAKEWORD(2, 2), &WSA)&&"Couldn`t create WSA on Server");
	ServerLogFile << "WSA started on Server!" << std::endl;
	ServerInfo.sin_family = AF_INET;
	ServerInfo.sin_port = htons(iPort);
	ServerInfo.sin_addr.s_addr = inet_addr(sIpAddress.c_str());
	assert(!((ListeningSocket = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR) && 
		"Couldn`t create Listening Socket on Server");
	ServerLogFile << "Listening socket created on server!" << std::endl;
	assert(!(bind(ListeningSocket, (const sockaddr*)&ServerInfo, iInfoSize))&&"Couldn`t bind Listening socket");
	ServerLogFile << "Listening socket binded!" << std::endl;
	listen(ListeningSocket, SOMAXCONN);
	ServerLogFile << "Server started at:" << inet_ntoa(ServerInfo.sin_addr) 
	<< ":" << ntohs(ServerInfo.sin_port) << std::endl;
	ServerLogFile << "Socket listens..." << std::endl;
}
void CServer::HandleClient(SOCKET ClientSocket){
	ServerLogFile << "Client connected with port:" << ntohs(ServerInfo.sin_port) << std::endl;
	while(true){
		if(recv(ClientSocket, szMainBuffer, BUFFER_SIZE, 0)>0){
			if (strstr(szMainBuffer, "file")) {
				recv(ClientSocket, szMainBuffer, BUFFER_SIZE, 0);
				std::string sFileName = std::string(szMainBuffer);
				SendFile(sFileName,ClientSocket);
				ZeroMemory(szMainBuffer, sizeof(szMainBuffer));
			}
			else {
				ServerLogFile<<"Received buffer from client with port:"<< 
				ntohs(ServerInfo.sin_port)<<" "<<szMainBuffer << std::endl;
				std::string tmpBuf(szMainBuffer);
				strcpy(szMainBuffer, tmpBuf.c_str());
				send(ClientSocket, szMainBuffer, BUFFER_SIZE, 0);
				ServerLogFile<<"Sent buffer to client "<<szMainBuffer<<std::endl;
			}
		}
		else{
			break;
		}
	}
}
void CServer::Start() {
	init();
	while (true) {
		SOCKET AcceptingSocket = accept(ListeningSocket, (sockaddr*)&ServerInfo, &iInfoSize);
		if(AcceptingSocket!=SOCKET_ERROR){
			Threads.push_back (std::thread (&CServer::HandleClient,this,AcceptingSocket));
			Threads.back().detach();
		}	
	}
}
void CServer::SendFile(std::string& sFileName,SOCKET& ClientSocket)
{
	using std::ios;
	std::ifstream File;
	File.open(sFileName, ios::binary | ios::ate);
	if (File.is_open()) {
		int iDataSize = File.tellg();
		File.close();
		File.open(sFileName, ios::binary);
		send(ClientSocket, std::to_string(iDataSize).c_str(), MESSAGE_SIZE, 0);
		for(;iDataSize>0;iDataSize-=BUFFER_SIZE){
			File.read(szMainBuffer,BUFFER_SIZE);
			send(ClientSocket,szMainBuffer,BUFFER_SIZE,0);
		}	
		File.close();
		ServerLogFile << "File was successfully sent to the client" << std::endl;
	}
	else {
		char ErrorBuffer[MESSAGE_SIZE] = "ERROR";
		send(ClientSocket, ErrorBuffer, MESSAGE_SIZE, 0);
	}
}