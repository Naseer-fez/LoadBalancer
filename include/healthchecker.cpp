#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <mutex>
#include <thread>
#include <chrono>
#include <thread>
#include <unordered_map>
#include "httplib.h"
#include "healthchecker.hpp"
#include "json.hpp"
#include "loadbalancer.hpp"
#define FILENAME "avaliableserver.txt"
#define CHECK "/health"
#define SLEEPTIME 12

using json = nlohmann::json;
std::mutex server_mutex;
// std::vector<std::string> SERVERS;

void readserverfile();
void Healthcheckerofservers();
void starthealththread();
std::vector<std::pair<std::string, float>> getallserver();
std::vector<std::pair<std::string, float>> SERVERS;
bool comparator(const std::pair<std::string, float> &a,
                const std::pair<std::string, float> &b);
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
    std::vector<std::pair<std::string, float>> newservers;
    while (std::getline(file, line))
    {
        // std::cout<<line<<"\n";
        newservers.push_back({line, 0.0f});
    }
    {

        for (const auto &[node, score] : newservers)
        {
            std::lock_guard<std::mutex> lock(server_mutex);
            {
                auto find = std::find_if(SERVERS.begin(), SERVERS.end(), [&](const auto &server)
                                         { return server.first == node;});
                if (find == SERVERS.end())
                {
                    SERVERS.push_back({node, score});
                }
            }
        }

    
    }
}
void Healthcheckerofservers()
{
    while (true)
    {
        std::vector<std::pair<std::string, float>> snapshot = {};
        bool change = false;
        {
            std::lock_guard<std::mutex> lock(server_mutex);
            snapshot = SERVERS;
        }

        for (size_t index = 0; index < snapshot.size();)
        {
            httplib::Client cli(snapshot[index].first);

            cli.set_connection_timeout(3, 0);

            auto response = cli.Get(CHECK); // can create a complex system aslo but lets reduce the complex code in this

            if (!response || response->status != 200)
            {
                std::cout << "[Health] DOWN : " << snapshot[index].first << '\n';
                snapshot[index] = std::move(snapshot.back());
                snapshot.pop_back();
                continue;
            }
            try
            {
                json body = json::parse(response->body);
                std::cout << "Response: " << body << '\n';
                std::unordered_map<std::string, float> health =
                    body.get<std::unordered_map<std::string, float>>();
                float score = givescore(health);
                snapshot[index].second = score;
                ++index;
            }
            catch (const std::exception &e)
            {
                std::cout << "[Health] INVALID : "
                          << snapshot[index].first << '\n';
                snapshot[index] = std::move(snapshot.back());
                snapshot.pop_back();
            }
        }

        std::sort(snapshot.begin(), snapshot.end(), comparator);
        std::lock_guard<std::mutex> lock(server_mutex);
        {
            SERVERS = snapshot;
        }

       
        { // optional for testing only
            std::lock_guard<std::mutex> lock(server_mutex);

            std::cout << "\nAlive Servers\n";

            for (const auto &server : SERVERS)
            {
                std::cout << "  " << server.first << '\n';
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

std::vector<std::pair<std::string, float>> getallserver()
{
    std::lock_guard<std::mutex> lock(server_mutex);
    {
        return SERVERS;
    }
}

bool comparator(const std::pair<std::string, float> &a,
                const std::pair<std::string, float> &b)
{
    return a.second > b.second;
}
