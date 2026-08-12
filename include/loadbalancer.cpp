#include<string>
#include<vector>

#include "healthchecker.hpp"

int movement = 0;
std::string giveaserver(std::vector<std::string> avaliableservers)
{

    movement++;
    return avaliableservers[movement % avaliableservers.size()];
}
