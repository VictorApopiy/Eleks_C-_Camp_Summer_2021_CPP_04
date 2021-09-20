#include "Client.h"
int main(){
    const std::string sServerAdrress = "127.0.0.1";
    const int iServerPort = 5500;
    Client client(sServerAdrress,iServerPort);
    client.ConnectToServer();
    return 0;
}