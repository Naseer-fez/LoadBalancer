#include<string>
#include<vector>

#include "healthchecker.hpp"

int movement = 0;
std::string giveaserver(std::vector<std::pair<std::string, std::vector<float>>> avaliableservers)
{

    movement++;
    return avaliableservers[movement % avaliableservers.size()].first;
}
