#include <string>
#include <vector>
#include <unordered_map>
#include "healthchecker.hpp"
#include <iostream>
#include <algorithm>
#include <mutex>
#include <cfloat>

extern std::vector<std::pair<std::string, float>> SERVERS;
extern std::mutex server_mutex;
float givescore(std::unordered_map<std::string, float> Score);
float weightoftheuser(float currentscore, float averagescore);
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

    // float score = it->second + weightoftheuser(it->second);
    // size_t index = std::distance(avaliableservers.begin(), it);
    float averagescore = 0;
    int index = 0;
    int size = avaliableservers.size();
    float min = FLT_MAX;
    for (int i = 0; i < size; i++)
    {
        float value = avaliableservers[i].second;
        averagescore += value;
        if (value <= min)
        {
            min = value;
            index = i;
        }
    }
    auto node = avaliableservers[index];
    float score = node.second + weightoftheuser(node.second, averagescore);
    {
        std::lock_guard<std::mutex> lock(server_mutex);
        SERVERS[index].second = score;
    }

    return node.first;
}

// Response: {"CPUUSAGE":80.61272395953432,"CPUCORES":7,"RAMUSAGE":56.41692569477845,"TOTALRAM":14}

// single source of changes in here
float givescore(std::unordered_map<std::string, float> Score)
{

    float cpuusage = Score.at("CPU");
    float cpucore = Score["CPUCORES"];

    float ramusage = Score.at("RAMUSAGE");
    float totalram = Score["TOTALRAM"];
    // Lower score = healthier server
    float cpuScore = 65.0f * ((100.0f - cpuusage) / cpucore);
    float ramScore = 45.0f * ((100.0f / totalram) - (ramusage / totalram));

    float score = (cpuScore + ramScore) * 0.1f;

    return score;

    // return finalscore;
}

float weightoftheuser(float currentscore, float averagescore)
{
    float difference = averagescore - currentscore;
    float values=(averagescore * 0.1f * difference) / averagescore;
    std::cout<<"\n[The values] is :"<<values<<"\n";
    return values;
    //This number is very import
}
/*

 float cpuusage = Score["CPUUSAGE"];
    float cpucore = Score["CPUCORES"];
    float ramusage = Score["RAMUSAGE"] * 1.0;
    float totalram = Score["TOTALRAM"] * 1.0;

    float cpupercore=(cpuusage/cpucore);
    float actualcpu=(100.0/cpucore);
    float CPU=actualcpu-cpupercore;
        cout<<CPU<<"\n";
    float rampergb=(ramusage/totalram);
    float actualram=(100.0/ramusage);
    float RAM=actualram-rampergb;

    float score=((CPU*55)+(RAM*45))*0.1;
*/