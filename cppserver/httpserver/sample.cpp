#include <iostream>
#include <string>
#include <sstream>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

int main()
{
    // ==========================
    // Initialize Winsock
    // ==========================
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        std::cout << "WSAStartup Failed\n";
        return 1;
    }

    // ==========================
    // Create Server Socket
    // ==========================
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET)
    {
        std::cout << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    // ==========================
    // Server Address
    // ==========================
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // ==========================
    // Bind
    // ==========================
    if (bind(serverSocket,
             (sockaddr*)&serverAddr,
             sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cout << "Bind failed\n";

        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // ==========================
    // Listen
    // ==========================
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cout << "Listen failed\n";

        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "HTTP Server running...\n";
    std::cout << "http://localhost:8080\n\n";

    while (true)
    {
        sockaddr_in clientAddr{};
        int clientSize = sizeof(clientAddr);

        SOCKET clientSocket =
            accept(serverSocket,
                   (sockaddr*)&clientAddr,
                   &clientSize);

        if (clientSocket == INVALID_SOCKET)
            continue;

        // ==========================
        // Receive Request
        // ==========================
        char buffer[4096];

        int bytesReceived =
            recv(clientSocket,
                 buffer,
                 sizeof(buffer) - 1,
                 0);

        if (bytesReceived <= 0)
        {
            closesocket(clientSocket);
            continue;
        }

        buffer[bytesReceived] = '\0';

        std::cout << "========== REQUEST ==========\n";
        std::cout << buffer << "\n";

        // ==========================
        // Parse First Line
        // ==========================
        std::string request(buffer);

        std::istringstream stream(request);

        std::string method;
        std::string path;
        std::string version;

        stream >> method >> path >> version;

        std::cout << "Method : " << method << '\n';
        std::cout << "Path   : " << path << '\n';
        std::cout << "Version: " << version << "\n\n";

        // ==========================
        // Create Response
        // ==========================
        std::string body;

        if (path == "/")
        {
            body =
                "<h1>Hello from C++ HTTP Server</h1>";
        }
        else if (path == "/about")
        {
            body =
                "<h1>About Page</h1>";
        }
        else
        {
            body =
                "<h1>404 Not Found</h1>";
        }

        std::string status =
            (path == "/" || path == "/about")
                ? "200 OK"
                : "404 Not Found";

        std::string response =
            "HTTP/1.1 " + status + "\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: " +
            std::to_string(body.size()) +
            "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            body;

        // ==========================
        // Send Response
        // ==========================
        send(clientSocket,
             response.c_str(),
             response.size(),
             0);

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}