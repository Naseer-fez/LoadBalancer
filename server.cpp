#include <iostream>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include <vector>
// Include the parser
#include "include/picohttpparser.h"
#pragma comment(lib, "Ws2_32.lib")

// Include the health checkers
#include "include/healthchecker.hpp"

// Includieng the the load balcner
#include "include/loadbalancer.hpp"
// g++ server.cpp include/healthchecker.cpp include/loadbalancer.cpp include/picohttpparser.c -Iinclude -lws2_32 -pthread -o server.exe
#define NEWHOST "/Host"

SOCKET startserver();
void ACCEPTLOOP(SOCKET serversocket);
void parsedata(const char *reqdata, size_t bytesrecived, SOCKET clientsocket);
void senddatatoserver(int parsedata, SOCKET clientsocket, bool client, const char *request, int reqlen);
void tranafertoserver(SOCKET clientsocket, const char *request, int reqlen);
void forwarddata(SOCKET clientsocket, SOCKET backednsocket);
bool communicatedata(SOCKET source, SOCKET destination);
int main()
{
    const SOCKET serversocket = startserver();
    starthealththread();
    ACCEPTLOOP(serversocket);
    // WSACleanup();
}

SOCKET startserver()
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
    return serversocket;
}

void ACCEPTLOOP(SOCKET serversocket)
{
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
            parsedata(buffer, bytesrecived, clientsocket);
        }
        else
        {
            closesocket(clientsocket);
        }
    }
}
void parsedata(const char *reqdata, size_t bytesrecived, SOCKET clientsocket)
{

    std::cout << "Recived\n";
    std::cout.write(reqdata, bytesrecived);

    const char *method;
    size_t method_len;
    const char *endpoint;
    size_t endpoint_len;
    int minor_version;
    phr_header headers[100];
    size_t num_headers = 100;

    int parsedata = phr_parse_request(
        reqdata,
        bytesrecived,
        &method, &method_len,
        &endpoint, &endpoint_len,
        &minor_version,
        headers, &num_headers,
        0);

    bool client = false;
    if (parsedata > 0)
    {
        std::string recivedendpoint(endpoint, endpoint_len);
        client = (recivedendpoint == NEWHOST);
    }

    senddatatoserver(parsedata, clientsocket, client, reqdata, bytesrecived);
}
void senddatatoserver(int parsedata, SOCKET clientsocket, bool client, const char *request, int reqlen)
{
    std::string response;

    if (parsedata == -2)
    {
        std::cout << "Incomplete HTTP request\n";
        std::string body = "400 Incomplete HTTP Request";
        response =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " +
            std::to_string(body.size()) + "\r\n"
                                          "Connection: close\r\n\r\n" +
            body;
    }
    else if (client)
    {
        response = "HTTP/1.1 200 OK\r\n\r\n";
        send(clientsocket, response.c_str(), (int)response.size(), 0);
        closesocket(clientsocket);
        return;
    }
    else if (parsedata == -1)
    {
        std::cout << "Invalid http request\n";
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
    if (parsedata < 0)
    {
        send(clientsocket, response.c_str(), (int)response.length(), 0);
        closesocket(clientsocket);
        return;
    }
    tranafertoserver(clientsocket, request, reqlen);
    return;
}

void tranafertoserver(SOCKET clientsocket, const char *request, int reqlen)
{

    std::string server = giveaserver();
    if (server.empty())
    {
        std::cout << "No servers available\n";
        std::string body = "503 Service Unavailable";
        std::string response =
            "HTTP/1.1 503 Service Unavailable\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " +
            std::to_string(body.size()) + "\r\n\r\n" + body;
        send(clientsocket, response.c_str(), (int)response.length(), 0);
        closesocket(clientsocket);
        return;
    }
    SOCKET backendsocket = socket(
        AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (backendsocket == INVALID_SOCKET)
    {
        std::cerr << "socket() failed\n";
        closesocket(clientsocket);
        return;
    }
    // Extract the ip
    size_t colon = server.find(':');
    std::string ip = server.substr(0, colon);       // get the ip
    int port = std::stoi(server.substr(colon + 1)); // string to int

    sockaddr_in backendadd{};
    backendadd.sin_family = AF_INET;
    backendadd.sin_port = htons(port);

    inet_pton(
        AF_INET,
        ip.c_str(),
        &backendadd.sin_addr);

    if (connect(backendsocket, reinterpret_cast<sockaddr *>(&backendadd), sizeof(backendadd)) == SOCKET_ERROR)
    {
        std::cerr << "connect() failed\n";
        std::string body = "502 Bad Gateway - Backend server unreachable";
        std::string response = "HTTP/1.1 503 Bad Gateway\r\n\r\n";
        send(clientsocket, response.c_str(), (int)response.size(), 0);
        closesocket(clientsocket);
        closesocket(backendsocket);
        return;
    }
    std::string backend_request(request, reqlen);
    size_t conn_pos = backend_request.find("Connection: keep-alive");
    if (conn_pos != std::string::npos)
    {
        backend_request.replace(conn_pos, 22, "Connection: close     ");
    }

    send(
        backendsocket,
        request,
        (reqlen), 0);

    forwarddata(clientsocket, backendsocket);
    closesocket(backendsocket);
    closesocket(clientsocket);
}
void forwarddata(SOCKET clientsocket, SOCKET backednsocket)
{

    while (1)
    {
        // Lets strt the fdset
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(clientsocket, &readfds);
        FD_SET(backednsocket, &readfds);
        timeval tv{0, 500000};

        int result = select(0, &readfds, nullptr, nullptr, &tv);
        // to counter infity timeout
        if (result <= 0)
        {
            break;
        }
        if (result == SOCKET_ERROR)
        {
            std::cerr << "select() failed\n";
            break;
        }

        if (FD_ISSET(clientsocket, &readfds))
        {
            if (!communicatedata(clientsocket, backednsocket))
            {
                break;
            }
        }

        if (FD_ISSET(backednsocket, &readfds))
        {

            if (!communicatedata(backednsocket, clientsocket))
            {
                break;
            }
    
        }

        // Now the data from the abckend here??
    }
}

bool communicatedata(SOCKET source, SOCKET destination)
{
    char buffer[8192];

    int bytesrecived = recv(
        source,
        buffer, sizeof(buffer) - 1,
        0);
    if (bytesrecived <= 0)
        return 0; // Nothing to get
    int sent = 0;
    while (sent < bytesrecived)
    {
        int n = send(
            destination,
            buffer + sent,
            (bytesrecived - sent), 0);
        if (n == SOCKET_ERROR)
            return 0;

        sent += n;
    }
    return 1;
}
