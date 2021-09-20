#include "Server.h"
int main(){
    const std::string sServerAdrress = "127.0.0.1";
    const int iServerPort = 5500;
    CServer server(sServerAdrress,iServerPort);
    server.Start();
    return 0;
}