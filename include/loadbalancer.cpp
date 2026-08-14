#include <string>
#include <vector>
#include <unordered_map>
#include "healthchecker.hpp"
#include <iostream>
#include <algorithm>
#include <mutex>
extern std::vector<std::pair<std::string, float>> SERVERS;
extern std::mutex server_mutex;

std::string giveaserver()
{
    std::vector<std::pair<std::string, float>> avaliableservers;
    {
        std::lock_guard<std::mutex> lock(server_mutex);
        avaliableservers = SERVERS;
    }

    if (avaliableservers.empty())
    {
        return "";
    }

    for (const auto &server : avaliableservers)
    {
        std::cout << "Server: " << server.first
                  << ", Score: " << server.second << '\n';
    }
    // return the min element
    auto it = std::min_element(
        avaliableservers.begin(),
        avaliableservers.end(),
        [](const auto &a, const auto &b)
        {
            return a.second < b.second;
        });
    float score = it->second + 5.0f;
    size_t index = std::distance(avaliableservers.begin(), it);
    {
        std::lock_guard<std::mutex> lock(server_mutex);
        SERVERS[index].second = score;
    }

    return it->first;
}

// Response: {"CPUUSAGE":80.61272395953432,"CPUCORES":7,"RAMUSAGE":56.41692569477845,"TOTALRAM":14}

// single source of changes in here
float givescore(std::unordered_map<std::string, float> Score)
{

    float cpuusage = Score["CPU"];
    int cpucores = Score["CPUCORES"];
    float ramusage = Score["RAMUSAGE"];
    int totalram = Score["TOTALRAM"];

    if (cpucores <= 0)
        cpucores = 1;

    float finalscore = 0;

    /*
    The toal assuming is that you cant have both the ram and cpu weight the same ,
    so for this reason , the cpu have to be given a little higher weight than usual

    */
    float CPU, RAM;
    float percoreusage = (cpucores * (cpuusage / 100.0f));

    CPU = percoreusage * (cpuusage / ((int(cpuusage) % cpucores) + cpucores));
    RAM = (totalram * (ramusage / 100.0f));

    finalscore = (CPU * 55 + RAM * 45) * 0.1;

    return finalscore;
}