#include "Server.h"
int main(){
    CServer s("127.0.0.1",1234);
    s.Start();
    return 0;
}