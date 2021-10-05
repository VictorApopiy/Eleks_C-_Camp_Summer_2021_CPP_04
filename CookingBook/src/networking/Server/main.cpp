#include "Server.h"
#define IP_ADDRESS "127.0.0.1"
#define PORT 1234
int main(){
    CServer server;
    if(!server.CheckIP()){
        server.SetServer(IP_ADDRESS,PORT);
    }
    server.Start();
    return 0;
}