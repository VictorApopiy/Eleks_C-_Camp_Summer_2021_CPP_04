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
    Time = std::time(0);
    CurrentTime = std::localtime(&Time);
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
    LogInfo("Client was set with ip:"+sIpAddress+":"+std::to_string(iPort));
}

std::string Client::ReceiveMessage(){
    ZeroMemory(szMainBuffer,g_iBufferSize);
    recv(ClientSocket,szMainBuffer,g_iBufferSize,0);
    LogInfo("Message received:"+std::string(szMainBuffer));
    return std::string(szMainBuffer);
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
    LogInfo("Client user was returned :"+ClientUser.m_sUserName);
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

bool Client::RecvFile(const std::string& sPath,const std::string& sFileName){
    bool bResult = false;
    send(ClientSocket, sFileName.c_str(),sFileName.size()+1, 0);

    LogInfo("Name of the file'"+ std::string(sFileName)+ "' was requested");
    char szTempSize[g_iMessageSize];
    recv(ClientSocket, szTempSize, g_iMessageSize, 0);

    if (strstr(szTempSize, "FILE_ERROR")) {
        std::cout << "Couldn`t open file on server" << std::endl;
        LogInfo("Couldn`t find file "+sFileName+" on server");
        return bResult;
    }

    int iRecvBufferSize = atoi(szTempSize);
    LogInfo("Size of the file "+sFileName+" was received:"+std::to_string(iRecvBufferSize));
    char *Buffer = new char[iRecvBufferSize];

    std::ofstream File;
    File.open(sPath + sFileName, std::ios::binary);
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
    InputFile.open(sFileName,std::ios::binary|std::ios::ate);
    iTmpRecvSize = InputFile.tellg();
    LogInfo("file size = "+std::to_string(iTmpRecvSize)+" received size = " + std::to_string(iRecvBufferSize));
    if(iTmpRecvSize!=iRecvBufferSize){
        send(ClientSocket,std::string(FILE_REQUEST).c_str(),
             strlen(std::string(FILE_REQUEST).c_str()),0);

        RecvFile(sPath,sFileName);
    }
    bResult = true;
    return bResult;
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
    int iVectorSize = static_cast<int>(Vector.size());
    std::cout<<"sending vector size"<<std::endl;
    send(ClientSocket,std::to_string(iVectorSize).c_str(),g_iMessageSize,0);
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
   /* char szVectorSize[g_iBufferSize];
    strcpy(szVectorSize,std::to_string(Vector.size()).c_str());
    send(ClientSocket,std::to_string(Vector.size()).c_str(),g_iBufferSize,0);
    for(size_t i = 0; i<Vector.size();i++){
        send(ClientSocket,Vector[i].c_str(),strlen(Vector[i].c_str()),0);
    }*/
}

bool Client::GetRecipesNeeded(std::vector<std::string>& RecipesNeeded,
    std::vector<std::string>&RecipesCategories,
    std::vector<std::string>&RecipesCategoryValues,
    std::vector<std::string>&RecipesIngredients){

    bool bResult = false;
    SendStringVector(RecipesCategories);
    Sleep(100);
    SendStringVector(RecipesCategoryValues);
    Sleep(100);
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
    send(ClientSocket,sRecipeName.c_str(),sRecipeName.size()+1,0);
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


std::vector<std::string> Client::ReceiveStringVector(){
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
   /* char szTmpSize[g_iBufferSize];
    char szTmpVector[g_iBufferSize];
    std::vector<std::string> sVector;
    recv(ClientSocket,szTmpSize,g_iBufferSize,0);
    for(int i = 0;i<atoi(szTmpSize);i++){
        recv(ClientSocket,szTmpVector,g_iBufferSize,0);
        sVector.push_back(std::string(szTmpVector));
    }
    return sVector;*/
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
    const char* szClearName = "CLEAR_NAME";

    if(sWord.size()==0){
        send(ClientSocket,szClearName, strlen(szClearName),0);
    }else{
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
    SendUser(User);
    LogInfo("Register request was sent with login: "+User.m_sUserName);
    recv(ClientSocket,ServerRespond,g_iMessageSize,0);
    if(strstr(ServerRespond,"USER_EXISTS")){
        LogInfo("USER_EXISTS message was received from server");
        std::cout<<"USER_EXISTS message was received from server"<<std::endl;
    }
    else{
        LogInfo("New "+ User.m_sUserName+" was added to main user data base");

        User  = ReceiveUser ();

        bResult = true;
    }
    return bResult;
}
void Client::SendUser(SUser&User){

    std::stringstream sDataStream;
    sDataStream<<std::to_string(User.m_nID)<<"\t"<<User.m_sFavorites<<"\t"
    <<User.m_sPassword<<"\t"<<User.m_sUserName<<"\t"<<std::endl;
    std::cout<<"sending size of buffer"<<std::endl;
    send(ClientSocket,std::to_string(strlen(sDataStream.str().c_str())).c_str(),g_iBufferSize,0);

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
    bool bResult = false;
    SendUser(User);
    char szServerRespond[g_iBufferSize];
    recv(ClientSocket,szServerRespond,g_iBufferSize,0);
    if(strstr(szServerRespond,"USER_DOESNT_EXIST")){
        LogInfo("Incorrect login or password : "+User.m_sUserName+  " " + User.m_sPassword);
        std::cout<<"Incorrect login or password : "+User.m_sUserName+  " " + User.m_sPassword<<std::endl;
    }
    else{
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
    LogInfo(sRequest + "request was sent to server");
}


void Client::SendFile(const std::string& sPath,const std::string &sFileName){
    using std::ios;
    std::ifstream File;
    send(ClientSocket,sFileName.c_str(),sFileName.size()+1,0);
    Sleep(100);
    File.open(sPath, ios::binary | ios::ate);
    if (File.is_open()) {
        int iDataSize = File.tellg();
        File.close();
        File.open(sPath, ios::binary);
        iDataSize++;
        send(ClientSocket, std::to_string(iDataSize).c_str(), g_iMessageSize, 0);
        char* Buffer = new char[iDataSize];
        File.read(Buffer,iDataSize);
        int iTmpDataSize = iDataSize;
        int iCount = 0;
        Sleep(100);
        while(iTmpDataSize>0){
            iCount = send(ClientSocket,Buffer,iDataSize,0);
            Sleep(100);
            if(iCount<0){
                break;
            }
            iTmpDataSize-=iCount;
        }
        if(Buffer){
            delete[] Buffer;
        }
        File.close();
        LogInfo("File " + sFileName +" was successfully sent to the server");
    }
    else {
        char szErrorBuffer[g_iMessageSize] = "FILE_ERROR";
        send(ClientSocket, szErrorBuffer, g_iMessageSize, 0);
    }
}


bool Client::AddRecipeToFavorites(const int iRecipeId){
    bool bResult = false;
    LogInfo("Add recipe to favourites func");
    send(ClientSocket,std::to_string(iRecipeId).c_str(),std::to_string(iRecipeId).size()+1,0);
    SendUser(ClientUser);
    char szAnswer[g_iBufferSize];
    recv(ClientSocket,szAnswer,g_iBufferSize,0);
    if(strstr(szAnswer,"RECIPE_ADDED")){
        LogInfo("Added recipe to list of favs of user");
        ClientUser = ReceiveUser();
        bResult = true;
    }else{
        LogInfo("Couldn't add recipe to favourites");
    }
    return bResult;
}

bool Client::GetFavouriteRecipes(std::vector<std::string>& sResultVector){
    bool bResult = false;
    if(ClientUser.m_sFavorites.size()==0){
        std::string sFavouritesClear = "FAVOURITES_CLEAR";
        send(ClientSocket,sFavouritesClear.c_str(),strlen(sFavouritesClear.c_str()),0);
    }else{
        LogInfo("Sending favourites list:"+ClientUser.m_sFavorites);
        send(ClientSocket,ClientUser.m_sFavorites.c_str(),strlen(ClientUser.m_sFavorites.c_str()),0);
        LogInfo("Favourites list was sent");
        char szAnswer[g_iBufferSize];
        recv(ClientSocket,szAnswer,g_iBufferSize,0);
        LogInfo("Received answer:"+std::string(szAnswer));
        if(strstr(szAnswer,"RECIPE_FOUND")){
            LogInfo("List of favourites was found on server");
            sResultVector = ReceiveStringVector();
            LogInfo("String vector of favourite recipes was received");
            bResult = true;
        }else{
            LogInfo("Recipes were not found on server:(");
        }
    }

    return bResult;
}


bool Client::DeleteRecipeFromFavourites(const int iRecipeId){
    bool bResult = false;
    LogInfo("DeleteRecipeFromFavourites: begin");
    LogInfo("sending recipe id...");
    send(ClientSocket,std::to_string(iRecipeId).c_str(),std::to_string(iRecipeId).size()+1,0);
    LogInfo("sending user...");
    SendUser(ClientUser);
    LogInfo("User was sent");
    char szAnswer[g_iBufferSize];
    recv(ClientSocket,szAnswer,g_iBufferSize,0);
    LogInfo("Answer received:"+std::string(szAnswer));
    if(strstr(szAnswer,/*CHANGE THIS*/"RECIPE_DELETED")){
        ClientUser = ReceiveUser();
        bResult = true;
    }else{
        LogInfo("Couldn't delete favourite recipe with id"+std::to_string(iRecipeId));
    }
    return bResult;
}

