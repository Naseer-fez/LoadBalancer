#ifndef LOADBALANCER_HPP
#define LOADBALANCER_HPP

#include <vector>
#include <string>
#include <utility>


std::string giveaserver(std::vector<std::pair<std::string, std::vector<float>>> avaliableservers);

#endif