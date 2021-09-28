#include "Client.h"

Client::Client() :
	sIpAddress(""),
	iPort(0),
	ClientSocket(INVALID_SOCKET),
	Time(std::time(0)),
	CurrentTime(std::localtime(&Time)){
	ZeroMemory(&ClientInfo, sizeof(ClientInfo));
	iClientInfoSize = sizeof(ClientInfo);
	ClientLogFile.open(sClientLogFileName,std::ios::binary|std::ios::app);
}

Client::~Client() {
	WSACleanup();
	closesocket(ClientSocket);
	if(ClientLogFile.is_open()){
		ClientLogFile.close();
	}
}

void Client::LogInfo(const std::string& sInfo){
	ClientLogFile<<"["<<CurrentTime->tm_year+1900<<":"<<
	CurrentTime->tm_mon+1<<":"<<
	CurrentTime->tm_mday<<":"<<
	CurrentTime->tm_hour<<":"<<
    CurrentTime->tm_min<<":"<<
	CurrentTime->tm_sec<<"]"<<" "+sInfo<<std::endl;
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
		LogInfo("Couldn`t connect to server");
		bResult = false;
	}
	else{
		LogInfo("Connection to server is successful");
		bResult = true;
	}
	return bResult;
}

void Client::RecvFile(const std::string& sFileName){
	
	send(ClientSocket, sFileName.c_str(),sFileName.size()+1, 0);

	LogInfo("Name of the file'"+ std::string(szMainBuffer)+ "' was requested");
	char szTempSize[g_iMessageSize];
	recv(ClientSocket, szTempSize, g_iMessageSize, 0);

	if (strstr(szTempSize, "FILE_ERROR")) {
		std::cout << "Couldn`t open file on server" << std::endl;
		LogInfo("Couldn`t find file "+sFileName+" on server");
		return;
	}

	int iRecvBufferSize = atoi(szTempSize);
	LogInfo("Size of the file "+sFileName+" was received:"+std::to_string(iRecvBufferSize));
	char *Buffer = new char[iRecvBufferSize];

	std::ofstream File;
	File.open(sFileName, std::ios::binary);
	int iTmpRecvSize = iRecvBufferSize;
	int iCount = 0;
	while(iTmpRecvSize>0){
		iCount = recv(ClientSocket,Buffer,iRecvBufferSize,0);
		if(iCount==-1)break;
		iTmpRecvSize-=iCount;
	}
	LogInfo("File input was received from server");
	File.write(Buffer,iRecvBufferSize);
	LogInfo("File was recreated on client successfully");
	delete[] Buffer;
	File.close();
	std::ifstream InputFile;
	InputFile.open(sFileName,std::ios::binary|std::ios::ate);
	iTmpRecvSize = InputFile.tellg();
	std::cout<<"file size = "<<iTmpRecvSize<<" received size = "<<iRecvBufferSize<<std::endl;
	if(iTmpRecvSize!=iRecvBufferSize){
		send(ClientSocket,"file",strlen("file"),0);
		RecvFile(sFileName);
	}
}

void Client::Init() {
	assert(!WSAStartup(MAKEWORD(2, 2), &WSA)&&"Couldn`t WSA on client");
	LogInfo("WSA started on client");
	
	ClientInfo.sin_addr.S_un.S_addr = inet_addr(sIpAddress.c_str());
	ClientInfo.sin_port = htons(iPort);
	ClientInfo.sin_family = AF_INET;
	assert(!((ClientSocket = socket(AF_INET , SOCK_STREAM, 0))==SOCKET_ERROR)&&"Couldn`t create socket on client");

	LogInfo("Client socket was created");
}
//this function is just for the test of server, the idea is that we will have various functions
// for different requests, so server will handle them
void Client::ConnectToServer() {
	if(TryConnection()){
		while (true) {
			std::cout << "Enter message" << std::endl;
			std::cin >> szMainBuffer;
			send(ClientSocket, szMainBuffer, strlen(szMainBuffer)+1, 0);
			LogInfo("Message was sent to server:"+  std::string(szMainBuffer));
			if (strstr(szMainBuffer, "file")) {
				std::cout << "Enter file name" << std::endl;
				std::cin >> szMainBuffer;
				RecvFile(std::string(szMainBuffer));
				ZeroMemory(szMainBuffer, sizeof(szMainBuffer));
			}
			if(strstr(szMainBuffer,"register")){
				SUser someuser;
				someuser.m_sUserName = "dadad";
				someuser.m_sPassword = "1234";
				someuser.m_sFavorites = "dada";
				someuser.m_nID = 1;
				RegisterUser(someuser);

			}
			if(strstr(szMainBuffer,"login")){
				SUser someuser;
				someuser.m_sUserName = "dadad";
				someuser.m_sPassword = "djak";
				someuser.m_sFavorites = "dada";
				someuser.m_nID = 1;
				LoginUser(someuser);
			}
			else {
				recv(ClientSocket, szMainBuffer, g_iBufferSize, 0);
				std::cout << "Message received:" << std::endl;
				std::cout << szMainBuffer << std::endl;
				LogInfo("Message was received from server:"+std::string(szMainBuffer));
			}
		}
	}
	else{
		std::cout<<"Can`t interact with server"<<std::endl;
		LogInfo("Couldn`t connect to server");
	}
} 

bool Client::RegisterUser(SUser& User){
	bool bResult;
	char ServerRespond[g_iMessageSize];
	send(ClientSocket,reinterpret_cast<char*>(&User),sizeof(User),0);
	LogInfo("Register request was sent with login: "+User.m_sUserName);
	recv(ClientSocket,ServerRespond,g_iMessageSize,0);
	if(strstr(ServerRespond,"USER_EXISTS")){
		LogInfo("USER_EXISTS message was received from server");
		std::cout<<"USER_EXISTS message was received from server"<<std::endl;
		bResult = false;
	}
	else{
		LogInfo("New"+ User.m_sUserName+" was added to main user data base");
		std::cout<<"New"+ User.m_sUserName+" was added to main user data base"<<std::endl;
		bResult = true;
	}
	return bResult;
}

bool Client::LoginUser(SUser& User){
	bool bResult;
	send(ClientSocket,reinterpret_cast<char*>(&User),sizeof(User),0);
	char szServerRespond[g_iBufferSize];
	recv(ClientSocket,szServerRespond,g_iBufferSize,0);
	if(strstr(szServerRespond,"USER_DOESNT_EXIST")){
		bResult = false;
		LogInfo("Incorrect login or password : "+User.m_sUserName+  " " + User.m_sPassword);
		std::cout<<"Incorrect login or password : "+User.m_sUserName+  " " + User.m_sPassword<<std::endl;
	}
	else{
		LogInfo("Login successful : "+User.m_sUserName + " " + User.m_sPassword);
		std::cout<<"Login successful : "+User.m_sUserName + " " + User.m_sPassword<<std::endl;
		bResult = true;
	}
	return bResult;
}
/*void Client::RequestStruct(SUser& User){
	char SomeBuffer[g_iBufferSize];
	send(ClientSocket,reinterpret_cast<char*>(&User),sizeof(User),0);
	recv(ClientSocket,SomeBuffer,g_iBufferSize,0);
	std::cout<<"Message received:"<<SomeBuffer<<std::endl;
}*/