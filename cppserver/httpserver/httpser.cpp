#include <iostream>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

int main()
{

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        std::cout << "Wsastarup failed\n";
        system("pause");
        return 1;
    }

    SOCKET serversocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serversocket == INVALID_SOCKET)
    {
        std::cout << "Socket creation failed\n";
        WSACleanup();
        system("pause");
        return 1;
    }
    sockaddr_in serveraddr{};
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8000);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(
            serversocket,
            (sockaddr *)&serveraddr,
            sizeof(serveraddr)) == SOCKET_ERROR)
    {
        std::cout << "Bind failed. Port 8000 might already be in use.\n";
        closesocket(serversocket);
        WSACleanup();
        system("pause");
        return 1;
    }

    if (listen(serversocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cout << "Listen failed\n";
        closesocket(serversocket);
        WSACleanup();
        system("pause");
        return 1;
    }
    std::cout << "HTTP Server running...\n";
    std::cout << "http://localhost:8000\n\n";
    while (true)
    {
        sockaddr_in clientaddr{};
        int cliendsize = sizeof(clientaddr);
        SOCKET clientsocket = accept(
            serversocket,
            (sockaddr *)&clientaddr,
            &cliendsize);
        if (clientsocket == INVALID_SOCKET)
            continue;

        char buffer[4096];
        int bytesrecived = recv(
            clientsocket,
            buffer, sizeof(buffer) - 1,
            0);
            
        if (bytesrecived > 0) {
            buffer[bytesrecived] = '\0';
            std::cout<<"Recived\n";
            std::cout<<buffer;
            
            // Extract the path from the HTTP request (e.g., "GET /about HTTP/1.1")
            std::istringstream request_stream(buffer);
            std::string method, path, protocol;
            request_stream >> method >> path >> protocol;

            // ==========================
            std::string body = "<h1>Hello,World</h1>";

            std::string status =
                (path == "/" || path == "/about")
                    ? "200 OK"
                    : "404 Not Found";

            // Using the dynamic response you commented out!
            std::string response =
                "HTTP/1.1 " + status + "\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: " + std::to_string(body.size()) + "\r\n"
                "Connection: close\r\n"
                "\r\n" +
                body;

            send(clientsocket, response.c_str(), (int)response.length(), 0);
        }
        
        closesocket(clientsocket);
    }
}
