#ifndef HEALTHCHECK_HPP
#define HEALTHCHECK_HPP


#include <vector>
#include <string>
#include <utility>

void readserverfile();
void Healthcheckerofservers();
void starthealththread();
std::vector<std::pair<std::string, float >>  getallserver();
#endif