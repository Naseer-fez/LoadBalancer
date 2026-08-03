#include<iostream>
#include<string>
#include<winsock2.h>
#include<ws2tcpip.h>

//g++ tcpsender.cpp -o tcpsender.exe -lws2_32   

#pragma comment(lib,"ws2_32.lib")


int main(){
WSADATA wsa;

if(WSAStartup(MAKEWORD(2,2),&wsa)!=0){

    std::cout<<"Wsa startup failed";
    return 1;
}
SOCKET  sendersocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

if(sendersocket==INVALID_SOCKET){
    std::cout<<"Socket connection failed\n";
    WSACleanup();
    return 1;
}

sockaddr_in senderadd{};
senderadd.sin_family=AF_INET;
senderadd.sin_port=htons(8000);
inet_pton(AF_INET,"127.0.0.1",&senderadd.sin_addr);
connect(sendersocket,(sockaddr*)&senderadd,sizeof(senderadd));

while (1){

std::string msg;
std::cout << "Enter the message to send (Server 2): ";
std::getline(std::cin, msg);
send(
    sendersocket,
    msg.c_str(),
    msg.size(),
0);
char buffer[1024];

int bytes=recv(
sendersocket,
buffer,
sizeof(buffer)-1,
0
);
if(bytes>0){
    buffer[bytes]='\0';
    std::cout<<"\nClient said:"<<buffer<<"\n";
}


}


closesocket(sendersocket);
}