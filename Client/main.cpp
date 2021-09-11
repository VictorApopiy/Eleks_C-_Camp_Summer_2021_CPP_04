#include "Client.h"
int main(){
    Client c("127.0.0.1",1234);
    c.ConnectToServer();
    return 0;
}