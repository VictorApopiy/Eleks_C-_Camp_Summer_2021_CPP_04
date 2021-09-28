#include "Server.h"
int main(){
    const std::string sServerAdrress = "127.0.0.1";
    const int iServerPort = 1234;
    CServer server;
    if(!server.CheckIP()){
        server.SetServer(sServerAdrress,iServerPort);
    }
    server.Start();
    return 0;
}