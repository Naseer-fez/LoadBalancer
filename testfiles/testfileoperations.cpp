#include <bits/stdc++.h>
#include "include/json.hpp"



#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <mutex>
#include <thread>
#include <chrono>
#include <thread>
#include "include/httplib.h"

#define FILENAME "avaliableserver.txt"
#define CHECK "/"
#define SLEEPTIME 12
std::mutex server_mutex;
std::set<std::string> SERVERS;
//$ g++ -static testlb.cpp -o testlb -O2 -lws2_32 -pthread
void readfile()
{

    std::ifstream file(FILENAME);
    if (!file)
    {
        std::cout << "File reading failed";
        system("pause");
        return;
    }
    std::string line;
    std::set<std::string> newservers;
    while (std::getline(file, line))
    {
        // std::cout<<line<<"\n";
        newservers.insert(line);
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

void Healthchecker()
{
    while (true)
    {
        std::set<std::string> snapshot;

        {
            std::lock_guard<std::mutex> lock(server_mutex);
            snapshot = SERVERS;
        }

        std::set<std::string> deadServers;

        for (const auto &node : snapshot)
        {
            httplib::Client cli(node);

            cli.set_connection_timeout(3, 0);

            auto response = cli.Get(CHECK);

            if (!response || response->status != 200)
            {
                std::cout << "[Health] DOWN : " << node << '\n';
                deadServers.insert(node);
            }
        }

        if (!deadServers.empty())
        {
            std::lock_guard<std::mutex> lock(server_mutex);

            for (const auto &node : deadServers)
            {
                SERVERS.erase(node);
            }
        }
        { //optional for testing only 
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





int main()
{
    
    readfile();
    std::cout << "Starting the thread::";
    std::thread health_check_thread(Healthchecker);
    // std::thread filereader_thread(readfile);
    // filereader_thread.join();
    while (true)
    {
        readfile();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    health_check_thread.join();
    return 0;
}