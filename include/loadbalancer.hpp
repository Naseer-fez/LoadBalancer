#ifndef LOADBALANCER_HPP
#define LOADBALANCER_HPP

#include <vector>
#include <string>
#include <utility>
#include<unordered_map>

std::string giveaserver();
float givescore(std::unordered_map<std::string,float> Score);
#endif