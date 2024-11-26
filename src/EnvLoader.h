
#ifndef ENVLOADER_H
#define ENVLOADER_H

#include <string>
#include <map>

std::map<std::string, std::string> loadEnv(const std::string& filename);

#endif // ENVLOADER_H