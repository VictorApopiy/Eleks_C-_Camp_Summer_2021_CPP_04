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

    std::cout<<"WSA started on Server!"<<std::endl;

	ServerInfo.sin_family = AF_INET;
	ServerInfo.sin_port = htons(iPort);
	ServerInfo.sin_addr.s_addr = inet_addr(sIpAddress.c_str());

	assert(!((ListeningSocket = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR) && 
		"Couldn`t create Listening Socket on Server");
	LogInfo("Listening socket created on server!");

    std::cout<<"Listening socket created on server!"<<std::endl;

	assert(!(bind(ListeningSocket, 
	reinterpret_cast<const sockaddr*>(&ServerInfo), iInfoSize))
	&&"Couldn`t bind Listening socket");

	LogInfo("Listening socket binded!");
	listen(ListeningSocket, SOMAXCONN);

    std::cout<<"Listening socket binded!"<<std::endl;

	LogInfo("Server started at:" + 
	 std::string(inet_ntoa(ServerInfo.sin_addr)) + ":" 
	 + std::to_string(ntohs(ServerInfo.sin_port)));

    std::cout<<"Server started at:"<< std::string(inet_ntoa(ServerInfo.sin_addr)) + ":"
               + std::to_string(ntohs(ServerInfo.sin_port))<<std::endl;

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

        std::cout<<"Recipe with id:"+std::to_string(iRecipeId) + " was deleted"<<std::endl;

	}
	else{
		send(ClientSocket,szRecipeNotDeleted,strlen(szRecipeNotDeleted),0);
		LogInfo("Recipe with id:"+std::to_string(iRecipeId) + " was NOT deleted");

        std::cout<<"Recipe with id:"+std::to_string(iRecipeId) + " was NOT deleted"<<std::endl;

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
                if(strstr(szMainBuffer,GET_FAVOURITE_RECIPES_REQUEST)){
                    LogInfo(std::string(GET_FAVOURITE_RECIPES_REQUEST)+" request was received");
                    SendFavouriteRecipes(ClientSocket);
                    break;
                }
                if(strstr(szMainBuffer,ADD_RECIPE_TO_FAVOURITES_REQUEST)){
                    LogInfo("'ADD_RECIPE_TO_FAVOURITES' request was received");
                    AddRecipeToFavouritesForClient(ClientSocket);
                    break;
                }
                if(strstr(szMainBuffer,SEND_FILE_REQUEST)){
                    LogInfo("'SEND_FILE request was received");
                    RecvFile(ClientSocket);
                    break;
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
    std::cout<<"Server was initialized"<<std::endl;
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

    if(strstr(szWord,"CLEAR_NAME")){
        std::cout<<"Clear name was received"<<std::endl;
        LogInfo("SendRecipesByName: Clear name was received");
    }
    else{
        int iVectorSize = 0;
        std::vector<std::string> szRecipes = RecipesDataBase::CRecipesDataBase::SearchByName(sWord,iVectorSize);
        if(szRecipes.size()!=0){
            send(ClientSocket,szRecipeFound,strlen(szRecipeFound),0);
            SendStringVector(szRecipes,ClientSocket);

            std::cout<<"Recipes were found by name:"<<sWord<<std::endl;
        }
        else{
            send(ClientSocket,szRecipeNotFound,strlen(szRecipeNotFound),0);
            std::cout<<"Recipes were NOT found by name:"<<sWord<<std::endl;
        }
    }
}

void CServer::SendFile(const std::string& sFileName,SOCKET& ClientSocket){
	using std::ios;
	std::ifstream File;
    File.open("D://CookingServer//CookingServer//Files//" + sFileName, ios::binary | ios::ate);
	if (File.is_open()) {
		int iDataSize = File.tellg();
		File.close();
        File.open("D://CookingServer//CookingServer//Files//"+sFileName, ios::binary);
        iDataSize++;
        send(ClientSocket, std::to_string(iDataSize).c_str(), g_iMessageSize, 0);
        char* Buffer = new char[iDataSize];
        File.read(Buffer,iDataSize);
		int iTmpDataSize = iDataSize;
        int iCount = 0;
        while(iTmpDataSize>0){
            iCount = send(ClientSocket,Buffer,iDataSize,0);
            Sleep(10);
            if(iCount<0){
                break;
            }
			iTmpDataSize-=iCount;
        }
		if(Buffer){
			delete[] Buffer;
		}
		File.close();
		LogInfo("File " + sFileName +" was successfully sent to the client");
        std::cout<<"File " + sFileName +" was successfully sent to the client"<<std::endl;
	}
	else {
		char szErrorBuffer[g_iMessageSize] = "FILE_ERROR";
		send(ClientSocket, szErrorBuffer, g_iMessageSize, 0);
        std::cout<<"There is no file on server with the name"<<sFileName<<" requested"<<std::endl;
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
        std::cout<<"Can't register user, his/her login already exists"<<std::endl;
	}
	else{
		UsersDataBase::CUsersDataBase::InsertData(ReceivedUser);
        send(ClientSocket,szUserAdded,strlen(szUserAdded),0);
		LogInfo("USER_ADDED message was sent to the client");

		ReceivedUser = UsersDataBase::CUsersDataBase::SelectUsersInfo(ReceivedUser.m_sUserName);
		SendUser(ReceivedUser,ClientSocket);
        std::cout<<"New User was registered!"<<std::endl;
		//send(ClientSocket,reinterpret_cast<char*>(&ReceivedUser),sizeof(ReceivedUser),0);
		std::cout<<"User id:"<<ReceivedUser.m_nID<<std::endl;
		bResult = true;
	}
	return bResult;
}

void CServer::SendUser(UsersDataBase::SUser&User,SOCKET&ClientSocket){

    std::stringstream sDataStream;
    sDataStream<<std::to_string(User.m_nID)<<"\t"<<User.m_sFavorites<<"\t"
    <<User.m_sPassword<<"\t"<<User.m_sUserName<<"\t"<<std::endl;
    std::cout<<"sending size of User's buffer"<<std::endl;
    send(ClientSocket,std::to_string(strlen(sDataStream.str().c_str())).c_str(),g_uBufferSize,0);

    std::cout<<"sending User's buffer"<<std::endl;
    send(ClientSocket,sDataStream.str().c_str(),strlen(sDataStream.str().c_str()),0);
    std::cout<<"sent User's buffer"<<std::endl;
}
UsersDataBase::SUser CServer::ReceiveUser(SOCKET&ClientSocket){
   char szTmpSize[g_uBufferSize];
    recv(ClientSocket,szTmpSize,g_uBufferSize,0);
    char* Buffer = new char[atoi(szTmpSize)+1];
    recv(ClientSocket,Buffer,atoi(szTmpSize),0);
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
    LogInfo("TryLoginUser");
	bool bResult = true;
	UsersDataBase::SUser ReceivedUser;
	std::cout<<"receiving user..."<<std::endl;
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
		std::cout<<"User logged with id:"<<ReceivedUser.m_nID<<std::endl;
		LogInfo("User logged with id:" + std::to_string(ReceivedUser.m_nID));
	}
	else{
        std::cout<<"User"<<ReceivedUser.m_sUserName<<" was not found"<<std::endl;

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

        std::cout<<"Recipe:"<<Recipe.m_sName<<" was changed!"<<std::endl;

		bResult = true;
	}
	else{
		send(ClientSocket,szRecipeNotChanged,strlen(szRecipeNotChanged),0);

        std::cout<<"Recipe with the nameL"<<Recipe.m_sName<<" wasn't changed"<<std::endl;
	}
	return bResult;
}

void CServer::SendStringVector(std::vector<std::string>&Vector, SOCKET& ClientSocket){
    std::stringstream sDataStream;
    int iVectorSize = static_cast<int>(Vector.size());
    std::cout<<"sending vector size"<<std::endl;
    send(ClientSocket,std::to_string(iVectorSize).c_str(),g_iMessageSize,0);
    Sleep(100);
    std::cout<<"sent size of vector in string:"<<std::to_string(iVectorSize).c_str()<<std::endl;

    for(size_t i = 0;i<Vector.size();i++){
             sDataStream<<Vector[i]<<"\t";
    }

    std::cout<<"buffer created:"<<std::endl;
    std::cout<<sDataStream.str()<<std::endl;
    int iDataSize = strlen(sDataStream.str().c_str());
    std::cout<<"size of buffer in string"<<std::to_string(iDataSize).c_str()<<std::endl;
    send(ClientSocket,std::to_string(iDataSize).c_str(),g_iMessageSize,0);
    std::cout<<"size sent"<<std::endl;
    //Sleep(100);
    std::cout<<"sending datastream"<<std::endl;
    send(ClientSocket,sDataStream.str().c_str(),iDataSize,0);
    std::cout<<"datastream sent"<<std::endl;
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
    Sleep(10);
    std::cout<<"Sending recipe..."<<std::endl;
	send(ClientSocket,RecipeDataStream.str().c_str(),strlen(RecipeDataStream.str().c_str()),0);
    std::cout<<"Recipe was sent!"<<std::endl;
}

bool CServer::SendRecipesNeededForClient(SOCKET&ClientSocket){
	bool bResult = false;
	std::vector<std::string> RecipesNeeded, RecipesCategories,
	RecipesCategoryValues,RecipesIngredients;
    std::cout<<"Receiving columns..."<<std::endl;
    RecipesCategories = ReceiveStringVector(ClientSocket);
    std::for_each(begin(RecipesCategories),end(RecipesCategories),[](const std::string& s){std::cout<<s<<std::endl;});
    std::cout<<"Receiving columns' values..."<<std::endl;
    RecipesCategoryValues = ReceiveStringVector(ClientSocket);
    std::for_each(begin(RecipesCategoryValues),end(RecipesCategoryValues),[](const std::string& s){std::cout<<s<<std::endl;});
    std::cout<<"Receiving ingredients..."<<std::endl;
    RecipesIngredients =  ReceiveStringVector(ClientSocket);
    std::for_each(begin(RecipesIngredients),end(RecipesIngredients),[](const std::string& s){std::cout<<s<<std::endl;});

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
        LogInfo("Recipes needed were found on server");
        std::cout<<"Recipes needed were found on server"<<std::endl;
	}else{
		send(ClientSocket,szRecipeNotFound,strlen(szRecipeNotFound),0);
        std::cout<<"There is no needed recipes in Server's DBs:("<<std::endl;
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
        std::cout<<"Recipes were found by owner's id:"<<iClientOwnersId<<std::endl;
		send(ClientSocket,szRecipeFound,strlen(szRecipeFound),0);
       // Sleep(100);
		SendStringVector(szRecipes,ClientSocket);
	}
	else{
        std::cout<<"Recipes were not found by owner's id:"<<iClientOwnersId<<std::endl;
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
    if(Recipe.m_nAmountOfPortions != 0){
        std::cout<<"Recipe was found by name:"<<(std::string(szRecipeName))<<std::endl;
		send(ClientSocket,szRecipeFound,strlen(szRecipeFound),0);
        Sleep(10);
        std::cout<<"Sending recipe..."<<std::endl;
		SendRecipe(Recipe,ClientSocket);
        std::cout<<"Recipe:"<<Recipe.m_sName<<" was sent!"<<std::endl;
	}
	else{
        std::cout<<"Recipe was not found by name:"<<(std::string(szRecipeName))<<std::endl;
		send(ClientSocket,szRecipeNotFound,strlen(szRecipeNotFound),0);
		bResult = false;
	}
	if(szRecipeName){
		delete[]szRecipeName;
	}
	return bResult;
}

std::vector<std::string> CServer::ReceiveStringVector(SOCKET ClientSocket){
    char szTmpSize[g_iMessageSize];
    char szVectorSize[g_iMessageSize];
    std::cout<<"recieving vector size"<<std::endl;
    recv(ClientSocket,szVectorSize,g_iMessageSize,0);
    std::cout<<"vector size received:"<<atoi(szVectorSize)<<std::endl;
    recv(ClientSocket,szTmpSize,g_iMessageSize,0);
    std::cout<<"size in string"<<std::endl;
    std::cout<<szTmpSize<<std::endl;
    std::cout<<"received buffer size"<<atoi(szTmpSize)<<std::endl;
    std::vector<std::string> sVector;
    if(atoi(szTmpSize)){
        char* szBuffer = new char[atoi(szTmpSize)];
        recv(ClientSocket,szBuffer,atoi(szTmpSize),0);
        std::stringstream sDataStream(szBuffer);
        std::string sTmp;
      //  std::cout<<sDataStream.str()<<std::endl;
        for(int i = 0; i<atoi(szVectorSize);i++){
            std::getline(sDataStream,sTmp,'\t');
            sVector.push_back(sTmp);
        }
       /* while(std::getline(sDataStream, sTmp,'\t')){
            sVector.push_back(sTmp);
        }*/
        if(szBuffer){
            delete[]szBuffer;
        }
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
    std::cout<<"Receiving recipe to add from client..."<<std::endl;
	RecipesDataBase::SRecipe Recipe  = ReceiveRecipe(ClientSocket);
    std::cout<<"Recipe received:"<<Recipe.m_sName<<std::endl;
	bool bResult = false;
	const char* szRecipeAdded = "RECIPE_ADDED";
	const char* szRecipeNotAdded = "RECIPE_NOT_ADDED";
	if(RecipesDataBase::CRecipesDataBase::InsertData(Recipe)){
		send(ClientSocket,szRecipeAdded,strlen(szRecipeAdded),0);
		std::cout<<Recipe.m_sName<<" was added"<<std::endl;
		bResult = true;
	}
	else{
        std::cout<<Recipe.m_sName<<" was not added"<<std::endl;
        send(ClientSocket,szRecipeNotAdded,strlen(szRecipeNotAdded),0);
	}
	return bResult;
}


void CServer::RecvFile(SOCKET&ClientSocket){

    //send(ClientSocket, sFileName.c_str(),sFileName.size()+1, 0);
    char szFileName[g_uBufferSize];
    recv(ClientSocket,szFileName,g_uBufferSize,0);
    std::string sFileName(szFileName);

    LogInfo("Name of the file'"+ std::string(szMainBuffer)+ "' was requested");
    char szTempSize[g_iMessageSize];
    recv(ClientSocket, szTempSize, g_iMessageSize, 0);

    if (strstr(szTempSize, "FILE_ERROR")) {
        std::cout << "Couldn`t open file on client" << std::endl;
        LogInfo("Couldn`t find file "+sFileName+" on client");
        return;
    }

    int iRecvBufferSize = atoi(szTempSize);
    LogInfo("Size of the file "+sFileName+" was received:"+std::to_string(iRecvBufferSize));
    char *Buffer = new char[iRecvBufferSize];

    std::ofstream File;
    File.open("D:\\CookingServer\\CookingServer\\Files\\"+sFileName, std::ios::binary);
    int iTmpRecvSize = iRecvBufferSize;
    int iCount = 0;
    while(iTmpRecvSize>0){
        iCount = recv(ClientSocket,Buffer,iRecvBufferSize,0);
        if(iCount<0){
            break;
        }
        iTmpRecvSize-=iCount;
    }
    LogInfo("File input was received from server");
    File.write(Buffer,iRecvBufferSize);
    LogInfo("File was recreated on client successfully");
    if(Buffer){
        delete[] Buffer;
    }
    File.close();
    std::ifstream InputFile;
    InputFile.open("D:\\CookingServer\\CookingServer\\Files\\"+sFileName,std::ios::binary|std::ios::ate);
    iTmpRecvSize = InputFile.tellg();
    InputFile.close();
    LogInfo("file size = "+std::to_string(iTmpRecvSize)+" received size = " + std::to_string(iRecvBufferSize));
}

bool CServer::AddRecipeToFavouritesForClient(SOCKET &ClientSocket){
    bool bResult = false;
    LogInfo("Add recipe to favs func:");
    char szRecipeName[g_uBufferSize];
    LogInfo("Receiving recipe name...");
    recv(ClientSocket,szRecipeName,g_uBufferSize,0);
    LogInfo("Name of the resipe received:"+std::string(szRecipeName));
    LogInfo("Receiving user...");
    UsersDataBase::SUser ReceivedUser = ReceiveUser(ClientSocket);
    LogInfo("User received:"+ReceivedUser.m_sUserName);
    RecipesDataBase::SRecipe sTmpRecipe =
            RecipesDataBase::CRecipesDataBase::SelectRecipeByName(std::string(szRecipeName));
    if(strstr(ReceivedUser.m_sFavorites.c_str(),std::to_string(sTmpRecipe.m_nId).c_str())){
        LogInfo("Recipe already exists in user's favourites!");
        const char* szRecipeAlreadyInList = "RECIPE_ALREADY_IN_LIST";
        send(ClientSocket,szRecipeAlreadyInList,strlen(szRecipeAlreadyInList),0);
    }
    else if(sTmpRecipe.m_nAmountOfPortions==0){
        LogInfo("There is no recipe with such name");
        const char* szRecipeNotFound = "RECIPE_NOT_FOUND";
        send(ClientSocket,szRecipeNotFound,strlen(szRecipeNotFound),0);
    }
    else{
        LogInfo("Recipe was found");
        const char* szResipeFound = "RECIPE_FOUND";
        send(ClientSocket,szResipeFound,strlen(szResipeFound),0);
        bResult = true;
        UsersDataBase::CUsersDataBase::AddToFavorites(ReceivedUser,sTmpRecipe.m_nId);
        LogInfo("Sending new favourites list to client...");
        send(ClientSocket,ReceivedUser.m_sFavorites.c_str(),strlen(ReceivedUser.m_sFavorites.c_str()),0);
        LogInfo("New list was sent:"+ReceivedUser.m_sFavorites);
    }
    return bResult;
}

bool CServer::SendFavouriteRecipes(SOCKET &ClientSocket){
    bool bResult = false;
    char szFavourites[g_uBufferSize];
    recv(ClientSocket,szFavourites,g_uBufferSize,0);
    if(strstr(szFavourites,"FAVOURITES_CLEAR")){
        LogInfo("SendFavouriteRecipes: Received clear favourites list");
    }else{
        std::string sFavorites(szFavourites);
        int iSizeOfCreatedList = 0;
        std::vector<std::string> Recipes = RecipesDataBase::CRecipesDataBase::SelectByID(sFavorites,iSizeOfCreatedList);
        if(iSizeOfCreatedList!=0){
            bResult = true;
            const char* szRecipeFound = "RECIPE_FOUND";
            send(ClientSocket,szRecipeFound,strlen(szRecipeFound),0);
            Sleep(100);
            SendStringVector(Recipes,ClientSocket);
        }else{
            const char* szRecipeNotFound = "RECIPE_NOT_FOUND";
            send(ClientSocket,szRecipeNotFound,strlen(szRecipeNotFound),0);
        }
    }
    return bResult;
}
