#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <mutex>
#include <thread>
#include <chrono>
#include <thread>
#include "include/httplib.h"
// #include "healthchecker.hpp"

#define FILENAME "avaliableserver.txt"
#define CHECK "/"
#define SLEEPTIME 12
std::mutex server_mutex;
std::vector<std::string> SERVERS;

void readserverfile();
void Healthcheckerofservers();
void starthealththread();
std::string giveaserver();



void readserverfile()
{

    std::ifstream file(FILENAME);
    if (!file)
    {
        std::cout << "File reading failed";
        system("pause");
        return;
    }
    std::string line;
    std::vector<std::string> newservers;
    while (std::getline(file, line))
    {
        // std::cout<<line<<"\n";
        newservers.push_back(line);
    }
    {
        std::lock_guard<std::mutex> lock(server_mutex);
        if (SERVERS != newservers)
        {
            SERVERS = std::move(newservers);
            std::cout << "[File] Server list updated.\n";
        }
    }
}
void Healthcheckerofservers()
{
    while (true)
    {
        std::vector<std::string> snapshot;

        {
            std::lock_guard<std::mutex> lock(server_mutex);
            snapshot = SERVERS;
        }

        std::vector<std::string> deadServers;

        for (const auto &node : snapshot)
        {
            httplib::Client cli(node);

            cli.set_connection_timeout(3, 0);

            auto response = cli.Get(CHECK);

            if (!response || response->status != 200)
            {
                std::cout << "[Health] DOWN : " << node << '\n';
                deadServers.push_back(node);
            }
        }

        if (!deadServers.empty())
        {
            std::lock_guard<std::mutex> lock(server_mutex);

            {
                for (const auto &server : deadServers)
                {
                    auto it = std::find(SERVERS.begin(), SERVERS.end(), server);
                    if (it != SERVERS.end())
                    {
                        SERVERS.erase(it);
                    }
                }
                std::sort(SERVERS.begin(), SERVERS.end());
            }
        }
        { // optional for testing only
            std::lock_guard<std::mutex> lock(server_mutex);

            std::cout << "\nAlive Servers\n";

            for (const auto &server : SERVERS)
            {
                std::cout << "  " << server << '\n';
            }

            std::cout << '\n';
        }

        std::this_thread::sleep_for(std::chrono::seconds(SLEEPTIME));
    }
}
void starthealththread()
{
    readserverfile();
    std::cout << "Starting the Health thread...\n";
    std::thread(Healthcheckerofservers).detach();
}
std::string giveaserver()
{
    
    return SERVERS[0];
}
