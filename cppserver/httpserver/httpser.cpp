#include <iostream>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>

// Include the parser
#include "include/picohttpparser.h"
#pragma comment(lib, "Ws2_32.lib")

// g++ httpser.cpp include/picohttpparser.c -Iinclude -lws2_32 -o httpser.exe

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

        char buffer[8192];
        int bytesrecived = recv(
            clientsocket,
            buffer, sizeof(buffer) - 1,
            0);

        if (bytesrecived > 0)
        {
            buffer[bytesrecived] = '\0';
            std::cout << "Recived\n";
            std::cout << buffer;
            // Now lets start with storing the individual lines
            // this stores the http headers
            const char *method;
            size_t method_len;
            // strore the endpoint
            const char *endpoint;
            size_t endpoint_len;
            // http version  the rules
            int minor_version;
            // http headers
            // 100 is for the buffer
            phr_header headers[100];
            size_t num_headers = 100;
            // Now the magic
            int parsethedata = phr_parse_request(
                buffer, // raw request
                bytesrecived,
                // now pass all the pointers
                &method, &method_len,
                &endpoint, &endpoint_len,
                &minor_version,
                headers, &num_headers,
                0
                // if it is first call always pass 0

            );
            std::string response;
            if (parsethedata > 0)
            {
                // BOMMMMMM
                std::cout << "\nBOMMMMM\n";
                std::string body = "Hello, World!";
                response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: " +
                    std::to_string(body.size()) + "\r\n"
                                                  "Connection: close\r\n"
                                                  "\r\n" +
                    body;
            }
            else if (parsethedata == -1)
            {
                std::cout << "Invalild http request\n";
                std::string body = "Bad Request";

                response =
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: " +
                    std::to_string(body.size()) + "\r\n"
                                                  "Connection: close\r\n"
                                                  "\r\n" +
                    body;
            }
            else if (parsethedata == -2)
            {
                std::cout << "Incomplete HTTP request\n";
                return 0;
            }

            send(clientsocket, response.c_str(), (int)response.length(), 0);
        }

        closesocket(clientsocket);
    }
}
