#include <string>
#include <vector>
#include <unordered_map>
#include "healthchecker.hpp"
#include<iostream>


std::string giveaserver(std::vector<std::pair<std::string, float>> avaliableservers)
{
    if (avaliableservers.empty())
    {
        return "";
    }

    for (const auto& server : avaliableservers) {
        std::cout << "Server: " << server.first
                  << ", Score: " << server.second << '\n';
    }
    return avaliableservers[0].first;
}

// Response: {"CPUUSAGE":80.61272395953432,"CPUCORES":7,"RAMUSAGE":56.41692569477845,"TOTALRAM":14}

//single source of changes in here
float givescore(std::unordered_map<std::string, float> Score)
{

    float cpuusage=Score["CPU"];
    int cpucores=Score["CPUCORES"];
    float ramusage=Score["RAMUSAGE"];
    int totalram=Score["TOTALRAM"];

    if (cpucores <= 0) cpucores = 1;
    
    float finalscore=0;

    /*
    The toal assuming is that you cant have both the ram and cpu weight the same , 
    so for this reason , the cpu have to be given a little higher weight than usual
    
    */
    float CPU,RAM;
    float percoreusage=(cpucores*(cpuusage/100.0f));
    
    CPU=percoreusage*(cpuusage/((int(cpuusage)%cpucores)+cpucores));
    RAM=(totalram*(ramusage/100.0f));
    
    finalscore=(CPU * 0.55f + RAM * 0.45f)*0.001;

    return cpuusage+ramusage;
}