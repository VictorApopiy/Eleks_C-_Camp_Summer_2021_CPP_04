#ifndef CClient
#define CClient

#include <string>
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <cassert>
#include <ctime>
#include "SUser.h"
#include <sstream>
#include <vector>
#include <algorithm>

#include "Requests.h"

#include "SRecipe.h"
const int g_iMessageSize  = 16;
const int g_iBufferSize  = 1024;
const std::string sIPAddressFileName = "IPAddressFile.bin";
const std::string sClientLogFileName = "ClientLogFile.bin";
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)
/*
I have changed the requests to server from commandline inputs to function MakeRequest that just sends the request
to server and server should handle it
I have tried everything with user input to commandline, everything was working fine but with the idea of having function
that sends request it stoped working
I don`t know what is the problem here
but code was working without troubles
*/
class Client {
private:
	WSAData WSA;
	char szMainBuffer[g_iBufferSize];
	SOCKET ClientSocket;
	sockaddr_in ClientInfo;
	int iClientInfoSize;
	std::string sIpAddress;
	int iPort;
	std::fstream IPAddressFile;
	std::fstream ClientLogFile;
	std::time_t Time;
	std::tm* CurrentTime;
	SUser ClientUser;
private:
	void Init();
	//char* GetUserPreparedForSending(SUser& User);

	void SendRecipe(SRecipe&Recipe);

	void SendUser(SUser&User);
	SUser ReceiveUser();

	SRecipe ReceiveRecipe(SOCKET &ClientSocket);
	void SendStringVector(std::vector<std::string>&Vector);
	std::vector<std::string> ReceiveStringVector();
	void LogInfo(const std::string& sInfo);
public:

	//Basic client operations
	Client();
	~Client();
	void RecvFile(const std::string&sFileName);
	bool CheckIP();
	void SetClient(const std::string& sIpaddress,const int iPort);
	bool TryConnection();
	void SetClientUser(const std::string& sUserName, const std::string& sPassword);

	//User operations
	void ConnectToServer();//should be deleted 
	bool RegisterUser(SUser& User);
	bool LoginUser(SUser& User);
	SUser& GetClientUser();

	//Recipe operations
	bool AddRecipeToServer(SRecipe& Recipe);
	bool FindRecipeByName(const std::string& sRecipeName,SRecipe&RecipeNeeded);
	bool GetRecipesNeeded(std::vector<std::string>& RecipesNeeded,
	std::vector<std::string>&RecipesCategories,
	std::vector<std::string>&RecipesCategoryValues,
	std::vector<std::string>&RecipesIngredients);

	bool GetRecipesByOwnersId(const int iOwnersId,std::vector<std::string>&Recipes);
	bool GetRecipesByName(const std::string& sWord,std::vector<std::string>&szRecipes);
	bool ChangeRecipe(SRecipe&Recipe);
	bool DeleteRecipe(const int iRecipeId);
	//Requests:

	void MakeRequest(const std::string& sRequest);
};

//
#endif //Client
//отче наш...