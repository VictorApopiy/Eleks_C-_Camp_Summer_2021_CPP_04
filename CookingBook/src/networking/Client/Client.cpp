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
	bool bResult = false;
	if(IPAddressFile.is_open()){
		if(IPAddressFile.tellg()!=0){
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
	return bResult;
}
SUser& Client::GetClientUser(){
	return ClientUser;
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
	if(Buffer){
		delete[] Buffer;
	}
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
SRecipe Client::ReceiveRecipe(SOCKET&ClientSocket){
	SRecipe RecipeResult;
	char szTmpSize[g_iMessageSize];
	recv(ClientSocket,szTmpSize,g_iMessageSize,0);
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
	
	
	if(Buffer){
		delete[]Buffer;
	}
	return RecipeResult;
}

void Client::SendStringVector(std::vector<std::string>&Vector){
	std::stringstream sDataStream;
	std::for_each(begin(Vector),end(Vector),[this,&sDataStream](const std::string& sValue){
		sDataStream<<sValue<<"\t";
	});
	LogInfo(sDataStream.str());
	send(ClientSocket,std::to_string(sDataStream.str().size()).c_str(),g_iMessageSize,0);
	

	send(ClientSocket,sDataStream.str().c_str(),sDataStream.str().size(),0);
}

bool Client::GetRecipesNeeded(std::vector<std::string>& RecipesNeeded,
	std::vector<std::string>&RecipesCategories,
	std::vector<std::string>&RecipesCategoryValues,
	std::vector<std::string>&RecipesIngredients){

	bool bResult = false;
	SendStringVector(RecipesCategories);
	SendStringVector(RecipesCategoryValues);
	SendStringVector(RecipesIngredients);
	char szAnswer[g_iBufferSize];
	recv(ClientSocket,szAnswer,g_iBufferSize,0);
	if(strstr(szAnswer,"RECIPE_FOUND")){
		RecipesNeeded = ReceiveStringVector();
		bResult = true;
		LogInfo("Requested recipes were found on server");
	}
	else{
		LogInfo("Requested recipes were NOT found on server");
	}
	return bResult;
}

bool Client::FindRecipeByName(const std::string& sRecipeName, SRecipe&RecipeNeeded){

	send(ClientSocket,std::to_string(sRecipeName.size()).c_str(),g_iMessageSize,0);
	send(ClientSocket,sRecipeName.c_str(),sRecipeName.size(),0);
	char szAnswer[g_iBufferSize];
	recv(ClientSocket,szAnswer,g_iBufferSize,0);
	bool bResult = false;
	if(strstr(szAnswer,"RECIPE_FOUND")){
		bResult = true;
		LogInfo("Recipe "+sRecipeName +"  was found on server");
		RecipeNeeded = ReceiveRecipe(ClientSocket);
	}
	else{
		LogInfo("Recipe "+sRecipeName+" was not found on server");
	}
	return bResult;

}
//should be deleted
void Client::ConnectToServer(){
	if(TryConnection()){
		while(true){
			std::string sMessage;
			std::cin>>sMessage;
			send(ClientSocket,sMessage.c_str(),sMessage.size(),0);
			if(strstr(sMessage.c_str(),GET_RECIPE_BY_NAME_REQUEST)){
				SRecipe rec;
				std::string name = "salad with tomatoes";
				FindRecipeByName(name,rec);
				
			}
			else{
				ZeroMemory(szMainBuffer,g_iBufferSize);
				recv(ClientSocket,szMainBuffer,g_iBufferSize,0);
				std::cout<<szMainBuffer<<std::endl;
			}
		}
	}
}

std::vector<std::string> Client::ReceiveStringVector(){
	char szTmpSize[g_iMessageSize];
	recv(ClientSocket,szTmpSize,g_iMessageSize,0);
	std::vector<std::string> sVector;
	if(atoi(szTmpSize)){
		char* szBuffer = new char[atoi(szTmpSize)];
		recv(ClientSocket,szBuffer,atoi(szTmpSize),0);
		std::stringstream sDataStream(szBuffer);
		std::string sTmp;
		while(std::getline(sDataStream,sTmp,'\t')){
			sVector.emplace_back(sTmp);
		}
		if(szBuffer){
			delete[]szBuffer;
		}
	}
	
	return sVector;
}

bool Client::GetRecipesByOwnersId(const int iOwnersId,std::vector<std::string>&szRecipes){
	bool bResult = false;
	send(ClientSocket, std::to_string(iOwnersId).c_str(),strlen(std::to_string(iOwnersId).c_str()),0);
	char szAnswer[g_iBufferSize];
	recv(ClientSocket,szAnswer,g_iBufferSize,0);
	if(strstr(szAnswer,"RECIPE_FOUND")){
		bResult = true;
		szRecipes = ReceiveStringVector();
		LogInfo("Recipes were found by owner`s id:" + std::to_string(iOwnersId));
	}
	else{
		LogInfo("Recipes were NOT found by owner`s id:" + std::to_string(iOwnersId));
	}
	return bResult;
}
bool Client::GetRecipesByName(const std::string& sWord,std::vector<std::string>&szRecipes){
	bool bResult = false;
	send(ClientSocket,sWord.c_str(),strlen(sWord.c_str()),0);
	char szAnswer[g_iBufferSize];
	recv(ClientSocket,szAnswer,g_iBufferSize,0);
	if(strstr(szAnswer,"RECIPE_FOUND")){
		bResult = true;
		szRecipes = ReceiveStringVector();
		LogInfo("Recipe/s was/were found on server(GetRecipeByName)");
	}
	else{
		LogInfo("Recipe/s was/were NOT found on server(GetRecipeByName)");
	}

	return bResult;
}

bool Client::ChangeRecipe(SRecipe&Recipe){
	SendRecipe(Recipe);
	char szAnswer[g_iBufferSize];
	recv(ClientSocket,szAnswer,g_iBufferSize,0);
	bool bResult = false;
	if(strstr(szAnswer,"RECIPE_CHANGED")){
		bResult = true;
		LogInfo("Recipe:"+Recipe.m_sName+" was changed on server");
	}
	else{
		LogInfo("Recipe:"+Recipe.m_sName+" was NOT changed on server");
	}
	return bResult;
}
bool Client::DeleteRecipe(const int iRecipeId){
	//send id
	send(ClientSocket,std::to_string(iRecipeId).c_str(),strlen(std::to_string(iRecipeId).c_str()),0);
	char szAnswer[g_iBufferSize];
	recv(ClientSocket,szAnswer,g_iBufferSize,0);
	bool bResult = false;
	if(strstr(szAnswer,"RECIPE_DELETED")){
		bResult = true;
		LogInfo("Recipe:"+std::to_string(iRecipeId)+" was deleted on server");
	}
	else{
		LogInfo("Recipe:"+std::to_string(iRecipeId)+" was NOT deleted on server");
	}
	return bResult;
}
bool Client::RegisterUser(SUser& User){
	bool bResult = false;
	char ServerRespond[g_iMessageSize];
	send(ClientSocket,	reinterpret_cast<char*>(&User),sizeof(User),0);
	LogInfo("Register request was sent with login: "+User.m_sUserName);
	recv(ClientSocket,ServerRespond,g_iMessageSize,0);
	if(strstr(ServerRespond,"USER_EXISTS")){
		LogInfo("USER_EXISTS message was received from server");
		std::cout<<"USER_EXISTS message was received from server"<<std::endl;
	}
	else{
		LogInfo("New "+ User.m_sUserName+" was added to main user data base");
		std::cout<<"New "+ User.m_sUserName+" was added to main user data base"<<std::endl;

		recv(ClientSocket,reinterpret_cast<char*>(&ClientUser),sizeof(ClientUser),0);
		std::cout<<"User id:"<<ClientUser.m_nID<<std::endl;
		bResult = true;
	}
	return bResult;
}  
void Client::SendUser(SUser&User){
	std::stringstream sDataStream;
	sDataStream<<User.m_nID<<"\t"<<User.m_sFavorites<<"\t"
	<<User.m_sPassword<<"\t"<<User.m_sUserName<<std::endl;
	std::cout<<"sending size of buffer"<<std::endl;
	std::string sBufferSize = std::to_string(sDataStream.str().size());
	std::cout<<"size of buffer on client:"<< sBufferSize<<std::endl;
	send(ClientSocket,sBufferSize.c_str(),sBufferSize.size(),0);
	std::cout<<"sending buffer"<<std::endl;
	send(ClientSocket,sDataStream.str().c_str(),strlen(sDataStream.str().c_str()),0);
	std::cout<<"sent buffer"<<std::endl;
}
SUser Client::ReceiveUser(){
	char szTmpSize[g_iBufferSize];
	recv(ClientSocket,szTmpSize,g_iBufferSize,0);
	char* Buffer = new char[atoi(szTmpSize)];
	recv(ClientSocket,Buffer,atoi(szTmpSize),0);
	std::stringstream sDataStream(Buffer);
	std::string sTmp;
	std::getline(sDataStream,sTmp,'\t');
	SUser User;
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
bool Client::LoginUser(SUser& User){
	std::cout<<"login user"<<std::endl;
	bool bResult = false;
	SendUser(User);
	char szServerRespond[g_iBufferSize];
	std::cout<<"waiting for respond..."<<std::endl;
	recv(ClientSocket,szServerRespond,g_iBufferSize,0);
	std::cout<<"server respond:"<<std::endl;
	if(strstr(szServerRespond,"USER_DOESNT_EXIST")){
		LogInfo("Incorrect login or password : "+User.m_sUserName+  " " + User.m_sPassword);
		std::cout<<"Incorrect login or password : "+User.m_sUserName+  " " + User.m_sPassword<<std::endl;
	}
	else{
		//recv(ClientSocket,reinterpret_cast<char*>(&User),sizeof(User),0);
		User = ReceiveUser();
		LogInfo("Login successful : "+
			User.m_sUserName + " "
			 + User.m_sPassword + " "
		 	 + std::to_string(User.m_nID));
		std::cout<<"Login successful : "+User.m_sUserName + " " + User.m_sPassword+" "
		+ std::to_string(User.m_nID)<<std::endl;
		bResult = true;
	}
	return bResult;
}
/*char* Client::GetUserPreparedForSending(SUser& User){
	return reinterpret_cast<char*>(&User);
}*/
void Client::SetClientUser(const std::string& sUserName, const std::string& sPassword){
	ClientUser.m_sUserName = sUserName;
	ClientUser.m_sPassword = sPassword;
}


void Client::SendRecipe(SRecipe&Recipe){
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
bool Client::AddRecipeToServer(SRecipe& Recipe){
	SendRecipe(Recipe);
	bool bResult = false;
	char szAnswer[g_iBufferSize];
	recv(ClientSocket,szAnswer,g_iBufferSize,0);
	if(strstr(szAnswer,"RECIPE_ADDED")){
		bResult = true;
	}
	return bResult;
}

void Client::MakeRequest(const std::string& sRequest){
	send(ClientSocket,sRequest.c_str(),strlen(sRequest.c_str()),0);
}


