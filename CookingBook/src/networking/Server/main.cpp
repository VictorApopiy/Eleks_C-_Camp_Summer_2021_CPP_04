#include "Server.h"
#define IP_ADDRESS "25.31.216.94"
#define PORT 1234
int main(){
    CServer server;
   /* if(!server.CheckIP()){
        server.SetServer(IP_ADDRESS,PORT);
    }*/
    server.SetServer(IP_ADDRESS,PORT);
    server.Start();
    return 0;
}
