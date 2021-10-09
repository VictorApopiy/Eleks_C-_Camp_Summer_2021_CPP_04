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
	Time = std::time(0);
	CurrentTime = std::localtime(&Time);
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

bool CServer::DeleteRecipe(SOCKET&ClientSocket){
	char szTmpSize[g_uBufferSize];
	recv(ClientSocket,szTmpSize,g_uBufferSize,0);
	int iRecipeId = atoi(szTmpSize);
	const char* szRecipeDeleted = "RECIPE_DELETED";
	const char* szRecipeNotDeleted = "RECIPE_NOT_DELETED";
	bool bResult = false;
	if(RecipesDataBase::CRecipesDataBase::DeleteRecipe(iRecipeId)){
		bResult = true;
		send(ClientSocket,szRecipeDeleted,strlen(szRecipeDeleted),0);
		LogInfo("Recipe with id:"+std::to_string(iRecipeId) + " was deleted");
	}
	else{
		send(ClientSocket,szRecipeNotDeleted,strlen(szRecipeNotDeleted),0);
		LogInfo("Recipe with id:"+std::to_string(iRecipeId) + " was NOT deleted");

	}
	return bResult;
}

void CServer::HandleClient(SOCKET ClientSocket){
	LogInfo("Client connected with port:" + std::to_string(ntohs(ServerInfo.sin_port)));
	std::cout<< "Client connected with port:" << ntohs(ServerInfo.sin_port) << std::endl;
	while(true){
		ZeroMemory(szMainBuffer,g_uBufferSize);
		if(recv(ClientSocket, szMainBuffer, g_uBufferSize, 0)>0){
			std::cout<<"Buffer received:"<<szMainBuffer<<std::endl;
			do{
				if(strstr(szMainBuffer,"vector")){
					std::vector<std::string> vec;
                    vec = ReceiveStringVector(ClientSocket);
					std::for_each(begin(vec),end(vec),[](const std::string& val){
						std::cout<<val<<"\t";
					});
					std::cout<<std::endl;
				}
				if (strstr(szMainBuffer, FILE_REQUEST)) {
					LogInfo("'File' request was sent");
					ZeroMemory(szMainBuffer,g_uBufferSize);
					recv(ClientSocket, szMainBuffer, g_uBufferSize, 0);
					std::string sFileName = std::string(szMainBuffer);
					LogInfo("file name received:" + sFileName);
					SendFile(sFileName,ClientSocket);
					ZeroMemory(szMainBuffer, sizeof(szMainBuffer));

					break;
				}
				if(strstr(szMainBuffer,REGISTER_REQUEST)){
					LogInfo("'Register' request was sent");
					if(TryRegisterUser(ClientSocket)){
						LogInfo("User registered successfully");
					}
					else{
						LogInfo("User couldn`t register");
					}

					break;
				}
				if(strstr(szMainBuffer,LOGIN_REQUEST)){
					LogInfo("'Login' request was sent");
					if(TryLoginUser(ClientSocket)){
						LogInfo("User logged in successfully");
					}
					else{
						LogInfo("User`s login or password was incorrect");
					}
					break;
				}
				if(strstr(szMainBuffer,GET_RECIPE_BY_NAME_REQUEST)){
					if(FindRecipeByNameFromClient(ClientSocket)){
						LogInfo("Recipe was found by name");
					}
					else{
						LogInfo("Recipe was not found by name");
					}
					break;
				}
				if(strstr(szMainBuffer,GET_RECIPES_NEEDED_REQUEST)){
					if(SendRecipesNeededForClient(ClientSocket)){
						LogInfo("Recipes needed were found");
					}
					else{
						LogInfo("Recipes needed were not found");
					}
					break;
				}
				if(strstr(szMainBuffer,ADD_RECIPE_REQUEST)){
					if(AddRecipeFromClient(ClientSocket)){
						LogInfo("User's Recipe was added successfully");
					}
					else{
						LogInfo("Couldn`t add user's recipe");
					}
					break;
				}
				if(strstr(szMainBuffer,GET_RECIPE_BY_OWNERS_ID_REQUEST)){
					SendRecipesByOwnersIdToClient(ClientSocket);
					break;
				}
				if(strstr(szMainBuffer,DELETE_RECIPE_REQUEST)){
					if(DeleteRecipe(ClientSocket)){
						LogInfo("User's Recipe was deleted successfully");
					}
					else{
						LogInfo("Couldn`t delete user's recipe");
					}
					break;
				}
				if(strstr(szMainBuffer,CHANGE_RECIPE_REQUEST)){
					if(ChangeRecipe(ClientSocket)){
						LogInfo("User's Recipe was changed successfully");
					}
					else{
						LogInfo("User's Recipe was not changed successfully");
					}
					break;
				}
				if(strstr(szMainBuffer,GET_RECIPES_BY_NAME_REQUEST)){
					SendRecipesByName(ClientSocket);
					break;
				}
				else {
					LogInfo("Unknown request was send from client");
					LogInfo("Received buffer from client with port:" + 
						std::to_string(ntohs(ServerInfo.sin_port)) + " " +
						std::string(szMainBuffer));
					std::string sTmp(szMainBuffer);
					strcpy(szMainBuffer,sTmp.c_str());
					send(ClientSocket,szMainBuffer,strlen(szMainBuffer),0);
				}
			}while(0);
		}
		else{
			LogInfo("Client disconnected from server");
			break;
		}
	}
}

void CServer::Start() {
	Init();
    UsersDataBase::CUsersDataBase::InitUsersDataBase
            ("Users.db");
    RecipesDataBase::CRecipesDataBase::InitRecipesDataBase
            ("Recipes.db");
	LogInfo("Server was initialized");
	while (true) {
		SOCKET AcceptingSocket = accept(ListeningSocket, reinterpret_cast<sockaddr*>(&ServerInfo), &iInfoSize);
		if(AcceptingSocket!=SOCKET_ERROR){
			Threads.push_back (std::thread (&CServer::HandleClient,this,AcceptingSocket));
			Threads.back().detach();
		}	
	}
}

void CServer::SendRecipesByName(SOCKET&ClientSocket){
	char szWord[g_uBufferSize];
	recv(ClientSocket,szWord,g_uBufferSize,0);
	std::string sWord(szWord);
	const char* szRecipeFound = "RECIPE_FOUND";
	const char* szRecipeNotFound = "RECIPE_NOT_FOUND";
	int iVectorSize = 0;
	std::vector<std::string> szRecipes = RecipesDataBase::CRecipesDataBase::SearchByName(sWord,iVectorSize);
	if(szRecipes.size()!=0){
		send(ClientSocket,szRecipeFound,strlen(szRecipeFound),0);
        SendStringVector(szRecipes,ClientSocket);
	}
	else{
		send(ClientSocket,szRecipeNotFound,strlen(szRecipeNotFound),0);
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
		iDataSize++;
		send(ClientSocket, std::to_string(iDataSize).c_str(), g_iMessageSize, 0);
		char* Buffer = new char[iDataSize];
		File.read(Buffer,iDataSize);
		int iTmpDataSize = iDataSize;
		int iCount = 0;
		while(iTmpDataSize>0){
			iCount = send(ClientSocket,Buffer,iDataSize,0);
			if(iCount==-1)break;
			iTmpDataSize-=iCount;
		}
		if(Buffer){
			delete[] Buffer;
		}
		File.close();
		LogInfo("File " + sFileName +" was successfully sent to the client");
	}
	else {
		char szErrorBuffer[g_iMessageSize] = "FILE_ERROR";
		send(ClientSocket, szErrorBuffer, g_iMessageSize, 0);
	}
} 
bool CServer::TryRegisterUser(SOCKET& ClientSocket){
	bool bResult = false;
	UsersDataBase::SUser ReceivedUser;
	//recv(ClientSocket,reinterpret_cast<char*>(&ReceivedUser),sizeof(ReceivedUser),0);
	ReceivedUser = ReceiveUser(ClientSocket);
	const char* szUserExists = "USER_EXISTS";
	const char* szUserAdded = "USER_ADDED";
	if(UsersDataBase::CUsersDataBase::CheckUsersExistence(ReceivedUser)){
		send(ClientSocket,szUserExists,strlen(szUserExists),0);
		LogInfo("USER_EXISTS message was sent to the client");
	}
	else{
		UsersDataBase::CUsersDataBase::InsertData(ReceivedUser);
        send(ClientSocket,szUserAdded,strlen(szUserAdded),0);
		LogInfo("USER_ADDED message was sent to the client");

		ReceivedUser = UsersDataBase::CUsersDataBase::SelectUsersInfo(ReceivedUser.m_sUserName);
		SendUser(ReceivedUser,ClientSocket);

		//send(ClientSocket,reinterpret_cast<char*>(&ReceivedUser),sizeof(ReceivedUser),0);
		std::cout<<"User id:"<<ReceivedUser.m_nID<<std::endl;
		bResult = true;
	}
	return bResult;
}

void CServer::SendUser(UsersDataBase::SUser&User,SOCKET&ClientSocket){
	std::stringstream sDataStream;
	sDataStream<<std::to_string(User.m_nID)<<"\t"<<User.m_sFavorites<<"\t"
	<<User.m_sPassword<<"\t"<<User.m_sUserName<<std::endl;
	send(ClientSocket,std::to_string(strlen(sDataStream.str().c_str())).c_str(),g_uBufferSize,0);
	send(ClientSocket,sDataStream.str().c_str(),strlen(sDataStream.str().c_str()),0);
}
UsersDataBase::SUser CServer::ReceiveUser(SOCKET&ClientSocket){
	char szTmpSize[g_uBufferSize];
	std::cout<<"receiving buffer size.."<<std::endl;
	recv(ClientSocket,szTmpSize,g_uBufferSize,0);
	std::cout<<"buffer size: "<<atoi(szTmpSize)<<std::endl;
	char* Buffer = new char[atoi(szTmpSize)];
	std::cout<<"receiving buffer.."<<std::endl;
	recv(ClientSocket,Buffer,atoi(szTmpSize),0);
	std::cout<<"buffer:"<<Buffer<<std::endl;
	std::stringstream sDataStream(Buffer);
	std::string sTmp;
	std::getline(sDataStream,sTmp,'\t');
	UsersDataBase::SUser User;
	User.m_nID = stoi(sTmp);
	std::getline(sDataStream,sTmp,'\t');
	User.m_sFavorites = sTmp;
	std::getline(sDataStream,sTmp,'\t');
	User.m_sPassword = sTmp;
	std::getline(sDataStream,sTmp,'\t');
	User.m_sUserName = sTmp;
	if(Buffer){
		delete[]Buffer;
	}
	return User;
}
bool CServer::TryLoginUser(SOCKET&ClientSocket){
	LogInfo("try login func");
	bool bResult = true;
	UsersDataBase::SUser ReceivedUser;
	std::cout<<"receiving user..."<<std::endl;
	//recv(ClientSocket,reinterpret_cast<char*>(&ReceivedUser),sizeof(ReceivedUser),0);
	ReceivedUser = ReceiveUser(ClientSocket);
	std::cout<<"User received"<<std::endl;
	std::cout<<ReceivedUser.m_sUserName<<std::endl;
	const char* szUserDoesntExist = "USER_DOESNT_EXIST";
	const char* szLoginSuccessful = "LOGIN_SUCCESSFUL";
	if(UsersDataBase::CUsersDataBase::CheckUsersPassword(ReceivedUser)){
		std::cout<<"user found"<<std::endl;
		send(ClientSocket,szLoginSuccessful,strlen(szLoginSuccessful),0);
		LogInfo("LOGIN_SUCCESSFUL message was sent to the client");

		ReceivedUser = UsersDataBase::CUsersDataBase::SelectUsersInfo(ReceivedUser.m_sUserName);
		SendUser(ReceivedUser,ClientSocket);
		//send(ClientSocket,reinterpret_cast<char*>(&ReceivedUser),sizeof(ReceivedUser),0);
		std::cout<<"User logged with id:"<<ReceivedUser.m_nID<<std::endl;
		LogInfo("User logged with id:" + std::to_string(ReceivedUser.m_nID));
	}
	else{
		std::cout<<"user not found"<<std::endl;

		send(ClientSocket,szUserDoesntExist,strlen(szUserDoesntExist),0);
		LogInfo("USER_DOESNT_EXIST message was sent to the client");
		bResult = false;
	}
	return bResult;
}

bool CServer::ChangeRecipe(SOCKET&ClientSocket){
	RecipesDataBase::SRecipe Recipe = ReceiveRecipe(ClientSocket);
	const char* szRecipeChanged = "RECIPE_CHANGED";
	const char* szRecipeNotChanged = "RECIPE_NOT_CHANGED";
	bool bResult = false;
	if(RecipesDataBase::CRecipesDataBase::UpdateRecipe(Recipe)){
		send(ClientSocket,szRecipeChanged,strlen(szRecipeChanged),0);
		bResult = true;
	}
	else{
		send(ClientSocket,szRecipeNotChanged,strlen(szRecipeNotChanged),0);
	}
	return bResult;
}

void CServer::SendStringVector(std::vector<std::string>&Vector, SOCKET& ClientSocket){
    char szVectorSize[g_uBufferSize];
    strcpy(szVectorSize,std::to_string(Vector.size()).c_str());
    send(ClientSocket,szVectorSize,strlen(szVectorSize),0);
    for(size_t i = 0; i<Vector.size();i++){
        send(ClientSocket,Vector[i].c_str(),strlen(Vector[i].c_str()),0);
    }
}
void CServer::SendRecipe(RecipesDataBase::SRecipe&Recipe,SOCKET&ClientSocket){
	std::stringstream RecipeDataStream;
	RecipeDataStream<<Recipe.m_bIsAccessible<<"\t"
	<<Recipe.m_dCalories<<"\t"<<Recipe.m_dWeigh<<"\t"
	<<Recipe.m_nAmountOfPortions<<"\t"<<Recipe.m_nId<<"\t"
	<<Recipe.m_nIdOfOwner<<"\t"<<Recipe.m_sAmountOfIngredients<<"\t"
	<<Recipe.m_sCategory<<"\t"<<Recipe.m_sCookingAlgorithm<<"\t"
	<<Recipe.m_sCuisine<<"\t"<<Recipe.m_sIngredients<<"\t"<<Recipe.m_sName<<"\t"
	<<Recipe.m_sUnitOfMeasurement;
	send(ClientSocket,std::to_string(strlen(RecipeDataStream.str().c_str())).c_str(),
	g_iMessageSize,0);
	send(ClientSocket,RecipeDataStream.str().c_str(),strlen(RecipeDataStream.str().c_str()),0);
}

bool CServer::SendRecipesNeededForClient(SOCKET&ClientSocket){
	bool bResult = false;
	std::vector<std::string> RecipesNeeded, RecipesCategories,
	RecipesCategoryValues,RecipesIngredients;
    RecipesCategories = ReceiveStringVector(ClientSocket);
    RecipesCategoryValues = ReceiveStringVector(ClientSocket);
    RecipesIngredients =  ReceiveStringVector(ClientSocket);
	int iRecipesNeededSize = 0;

	RecipesNeeded = RecipesDataBase::CRecipesDataBase::SelectRecipes(RecipesCategories,
	RecipesCategoryValues,
	RecipesIngredients,
	iRecipesNeededSize);
	const char* szRecipeFound = "RECIPE_FOUND";
	const char* szRecipeNotFound = "RECIPE_NOT_FOUND";
	if(RecipesNeeded.size()!=0){
		send(ClientSocket,szRecipeFound,strlen(szRecipeFound),0);
		SendStringVector(RecipesNeeded,ClientSocket);
		bResult = true;

	}else{
		send(ClientSocket,szRecipeNotFound,strlen(szRecipeNotFound),0);
	}

	return bResult;
}

void CServer::SendRecipesByOwnersIdToClient(SOCKET&ClientSocket){
	char szTmpSize[g_uBufferSize];
	recv(ClientSocket,szTmpSize,g_uBufferSize,0);
	int iClientOwnersId = atoi(szTmpSize);
	int iSizeOfCreatedList = 0;
	std::vector<std::string> szRecipes;
	szRecipes = RecipesDataBase::CRecipesDataBase::SelectRecipesByOwnersID(iClientOwnersId,iSizeOfCreatedList);
	const char* szRecipeFound = "RECIPE_FOUND";
	const char* szRecipeNotFound = "RECIPE_NOT_FOUND";
	if(szRecipes.size()!=0){
		send(ClientSocket,szRecipeFound,strlen(szRecipeFound),0);
		SendStringVector(szRecipes,ClientSocket);
	}
	else{
		send(ClientSocket,szRecipeNotFound,strlen(szRecipeNotFound),0);
	}
}

bool CServer::FindRecipeByNameFromClient(SOCKET&ClientSocket){
	char szTmpSize[g_iMessageSize];
	recv(ClientSocket,szTmpSize,g_iMessageSize,0);
	char* szRecipeName = new char[atoi(szTmpSize)];
	recv(ClientSocket,szRecipeName,atoi(szTmpSize),0);
	const char* szRecipeFound = "RECIPE_FOUND";
	const char* szRecipeNotFound = "RECIPE_NOT_FOUND";
	bool bResult = true;
	RecipesDataBase::SRecipe Recipe	 = RecipesDataBase::CRecipesDataBase::SelectRecipeByName(std::string(szRecipeName));
	if(Recipe.m_dCalories != 0){
		send(ClientSocket,szRecipeFound,strlen(szRecipeFound),0);
		SendRecipe(Recipe,ClientSocket);
	}
	else{
		send(ClientSocket,szRecipeNotFound,strlen(szRecipeNotFound),0);
		bResult = false;
	}
	if(szRecipeName){
		delete[]szRecipeName;
	}
	return bResult;
}

std::vector<std::string> CServer::ReceiveStringVector(SOCKET ClientSocket){
    char szTmpSize[g_uBufferSize];
    char szTmpVector[g_uBufferSize];
    std::vector<std::string> sVector;
    recv(ClientSocket,szTmpSize,g_uBufferSize,0);
    std::cout<<"received buffer size :"<<atoi(szTmpSize)<<std::endl;
    for(int i = 0;i<atoi(szTmpSize);i++){
        recv(ClientSocket,szTmpVector,g_uBufferSize,0);
        sVector.push_back(std::string(szTmpVector));
        ZeroMemory(szTmpVector,g_uBufferSize);
    }
    return sVector;
}
RecipesDataBase::SRecipe CServer::ReceiveRecipe(SOCKET&ClientSocket){
	RecipesDataBase::SRecipe RecipeResult;
	char szTmpSize[g_iMessageSize];
	recv(ClientSocket,szTmpSize,g_iMessageSize,0);
	std::cout<<"Size = "<<atoi(szTmpSize)<<std::endl;;
	char *Buffer = new char[atoi(szTmpSize)];
	recv(ClientSocket,Buffer,atoi(szTmpSize),0);
	std::stringstream RecipeDataStream(Buffer);
	std::string sTmp;
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_bIsAccessible = stoi(sTmp);
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_dCalories = stod(sTmp);
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_dWeigh = stod(sTmp);
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_nAmountOfPortions = stoi(sTmp);
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_nId = stoi(sTmp);
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_nIdOfOwner  =stoi(sTmp);
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_sAmountOfIngredients = sTmp;
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_sCategory = sTmp;
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_sCookingAlgorithm = sTmp;
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_sCuisine = sTmp;
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_sIngredients = sTmp;
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_sName =sTmp;
	std::getline(RecipeDataStream,sTmp,'\t');
	RecipeResult.m_sUnitOfMeasurement = sTmp;
	
	std::cout<<RecipeResult.m_sName<<std::endl;
	if(Buffer){
		delete[]Buffer;
	}
	return RecipeResult;
}

bool CServer::AddRecipeFromClient(SOCKET&ClientSocket){
	RecipesDataBase::SRecipe Recipe  = ReceiveRecipe(ClientSocket);
	bool bResult = false;
	const char* szRecipeAdded = "RECIPE_ADDED";
	const char* szRecipeNotAdded = "RECIPE_NOT_ADDED";
	if(RecipesDataBase::CRecipesDataBase::InsertData(Recipe)){
		send(ClientSocket,szRecipeAdded,strlen(szRecipeAdded),0);
		std::cout<<Recipe.m_sName<<" was added"<<std::endl;
		bResult = true;
	}
	else{
		send(ClientSocket,szRecipeNotAdded,strlen(szRecipeNotAdded),0);
	}
	return bResult;
}
