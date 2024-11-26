
#include "EnvLoader.h"
#include <fstream>
#include <sstream>

std::map<std::string, std::string> loadEnv(const std::string& filename) {
    std::map<std::string, std::string> envMap;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                envMap[key] = value;
            }
        }
    }

    return envMap;
}