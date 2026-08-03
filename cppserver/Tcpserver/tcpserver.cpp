#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h> //optional but contians high level featuers

#pragma comment(lib, "Ws2_32.lib")
// g++ tcpserver.cpp -o tcpserver.exe -lws2_32

int main()
{

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {

        std::cout << "WSA Satrtup failed";
        return 1;
    }
    SOCKET serversocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    /*
    AF_ -->Measn ddres form of internet, that is ipv 4
    sockstream means the method of streaming used
    iportot tcp -->the method used for transverfingt he data
    */
    if (serversocket == INVALID_SOCKET)
    {
        std::cout << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{}; // Empty strcut
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(
            serversocket,
            reinterpret_cast<sockaddr *>(&serverAddr),
            sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cout << "Bind failed";
        closesocket(serversocket);
        WSACleanup();
    }
    // the server is created

    if (listen(serversocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cout << "Listen failed\n";
        closesocket(serversocket);
        WSACleanup();
        return 1;
    }
    // server is running now HUARRY
    std::cout << "=====================================\n";
    std::cout << "TCP Server Running\n";
    std::cout << "Listening on port 8000...\n";
    std::cout << "=====================================\n";

    while (1)
    {
        sockaddr_in clientAdr{};
        int clientsize = sizeof(clientAdr);
        SOCKET clientSocket = accept(
            serversocket,
            reinterpret_cast<sockaddr *>(&clientAdr),
            &clientsize);
        if (clientSocket == INVALID_SOCKET)
        {

            std::cout << "Acceptated failure\n";
            closesocket(serversocket);
            WSACleanup();
            return 1;
        }
        while(2)
        {char buffer[1024];

        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0); // write the data in the buffer

        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0';

            std::cout << "Received:\n";
            std::cout << buffer << "\n\n";
            // 8. Reply
            std::string reply = "Hello Client! Message received.";
            // std::string reply;
            // std::cout << "Enter the message to send:(Server 1) ";
            // std::getline(std::cin, reply);
            send(
                clientSocket,
                reply.c_str(),
                static_cast<int>(reply.size()),
                0);
        }}
    }
    // 9. Close sockets
    // closesocket(clientSocket);
    // closesocket(serversocket);

    // // 10. Cleanup Winsock
    // WSACleanup();

    return 0;
}