#include "Client.h"
int main(){
    Client client;
    std::string sIpAddress = "192.168.56.1";
    int iPort = 1234;
    if(!client.CheckIP()){
        client.SetClient(sIpAddress,iPort);
    }
    client.ConnectToServer();
    return 0;
}