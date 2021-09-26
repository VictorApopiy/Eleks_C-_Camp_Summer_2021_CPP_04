#include "Server.h"

CServer::CServer() :
	ListeningSocket(INVALID_SOCKET),
	sIpAddress(""),
	iPort(0) ,
	Time(std::time(0)),
	CurrentTime(std::localtime(&Time)){
	ZeroMemory(&ServerInfo, sizeof(ServerInfo));
	iInfoSize = sizeof(ServerInfo);
	ServerLogFile.open(g_sServerLogFileName,std::ios::binary|std::ios::app);
}
CServer::~CServer(){
	WSACleanup();
	closesocket(ListeningSocket);
	if(ServerLogFile.is_open()){
		ServerLogFile.close();
	}
}
void CServer::LogInfo(const std::string& sInfo){
	ServerLogFile<<"["<<CurrentTime->tm_year+1900<<":"<<
	CurrentTime->tm_mon+1<<":"<<
	CurrentTime->tm_mday<<":"<<
	CurrentTime->tm_hour<<":"<<
    CurrentTime->tm_min<<":"<<
	CurrentTime->tm_sec<<"]"<<" "+sInfo<<std::endl;
}
void CServer::SetServer(const std::string& sIpAddress, const int iPort){
	this->sIpAddress = sIpAddress;
	this->iPort = iPort;
	IPAddressFile.open(sIPAddressFileName,std::ios::binary|std::ios::out|std::ios::trunc);
	IPAddressFile<<sIpAddress<<std::endl;
	IPAddressFile<<iPort;
	IPAddressFile.close();
}
bool CServer::CheckIP(){
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
void CServer::Init() {

	assert(!WSAStartup(MAKEWORD(2, 2), &WSA)&&
	"Couldn`t create WSA on Server");
	LogInfo("WSA started on Server!");

	ServerInfo.sin_family = AF_INET;
	ServerInfo.sin_port = htons(iPort);
	ServerInfo.sin_addr.s_addr = inet_addr(sIpAddress.c_str());

	assert(!((ListeningSocket = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR) && 
		"Couldn`t create Listening Socket on Server");
	LogInfo("Listening socket created on server!");

	assert(!(bind(ListeningSocket, 
	reinterpret_cast<const sockaddr*>(&ServerInfo), iInfoSize))
	&&"Couldn`t bind Listening socket");

	LogInfo("Listening socket binded!");
	listen(ListeningSocket, SOMAXCONN);

	LogInfo("Server started at:" + 
	 std::string(inet_ntoa(ServerInfo.sin_addr)) + ":" 
	 + std::to_string(ntohs(ServerInfo.sin_port)));

	LogInfo("Server listens...");
}
void CServer::HandleClient(SOCKET ClientSocket){
	LogInfo("Client connected with port:" + std::to_string(ntohs(ServerInfo.sin_port)));
	std::cout<< "Client connected with port:" << ntohs(ServerInfo.sin_port) << std::endl;
	while(true){
		if(recv(ClientSocket, szMainBuffer, g_uBufferSize, 0)>0){
			if (strstr(szMainBuffer, "file")) {
				recv(ClientSocket, szMainBuffer, g_uBufferSize, 0);
				std::string sFileName = std::string(szMainBuffer);

				SendFile(sFileName,ClientSocket);
				ZeroMemory(szMainBuffer, sizeof(szMainBuffer));
			}
			else {
				LogInfo("Received buffer from client with port:" + 
					std::to_string(ntohs(ServerInfo.sin_port)) + " " +
				 	std::string(szMainBuffer));
				
				std::string tmpBuf(szMainBuffer);
				strcpy(szMainBuffer, tmpBuf.c_str());
				send(ClientSocket, szMainBuffer, strlen(szMainBuffer)+1, 0);

				LogInfo("Sent buffer to client "+ std::string(szMainBuffer));
			}
		}
		else{
			break;
		}
	}
}
void CServer::Start() {
	Init();
	while (true) {
		SOCKET AcceptingSocket = accept(ListeningSocket, reinterpret_cast<sockaddr*>(&ServerInfo), &iInfoSize);
		if(AcceptingSocket!=SOCKET_ERROR){
			Threads.push_back (std::thread (&CServer::HandleClient,this,AcceptingSocket));
			Threads.back().detach();
		}	
	}
}
void CServer::SendFile(const std::string& sFileName,SOCKET& ClientSocket){
	using std::ios;
	std::ifstream File;
	File.open(sFileName, ios::binary | ios::ate);
	if (File.is_open()) {
		int iDataSize = File.tellg();
		File.close();
		File.open(sFileName, ios::binary);
		send(ClientSocket, std::to_string(iDataSize).c_str(), g_iMessageSize, 0);
		char* Buffer = new char[iDataSize];
		File.read(Buffer,iDataSize);
		send(ClientSocket,Buffer,iDataSize,0);

		delete Buffer;
		File.close();
		LogInfo("File " + sFileName +" was successfully sent to the client");
	}
	else {
		char szErrorBuffer[g_iMessageSize] = "ERROR";
		send(ClientSocket, szErrorBuffer, g_iMessageSize, 0);
	}
} 