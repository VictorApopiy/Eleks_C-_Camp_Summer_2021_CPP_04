#include "Client.h"
#define IP_ADDRESS "127.0.0.1"
#define PORT 1234
int main(){

    
    Client client;
    if(!client.CheckIP()){
        client.SetClient(IP_ADDRESS,PORT);
    }
    client.ConnectToServer();
    return 0;
}