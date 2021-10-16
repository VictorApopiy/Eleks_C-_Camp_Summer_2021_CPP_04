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

	void SendRecipe(SRecipe&Recipe);

	void SendUser(SUser&User);
	SUser ReceiveUser();

	SRecipe ReceiveRecipe(SOCKET &ClientSocket);

	std::vector<std::string> ReceiveStringVector();
	void LogInfo(const std::string& sInfo);
    void SendStringVector(std::vector<std::string>&Vector);
public:

	//Basic client operations
	Client();
	~Client();
	void RecvFile(const std::string&sFileName);
	bool CheckIP();
	void SetClient(const std::string& sIpaddress,const int iPort);
	bool TryConnection();
	void SetClientUser(const std::string& sUserName, const std::string& sPassword);
    void SendFile(const std::string& sFileName);
	//User operations
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

    bool AddRecipeToFavorites(const std::string sRecipeName);
    bool GetFavouriteRecipes(std::vector<std::string>& sResultVector);

    //the single function that  doesn`t need request before the function call
    bool DeleteRecipeFromFavourites(const std::string sRecipeName);
	//Requests:

    void MakeRequest(const std::string& sRequest);
    std::string ReceiveMessage();
};

//
#endif //Client
//отче наш...

