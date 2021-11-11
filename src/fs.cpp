#include "fs.h"
#include <stdexcept>
std::string FS::readFile(filesystem::path path) {
    std::cout << "Reading from " << path << std::endl;
    std::ifstream file {path};
    if (file.is_open()) {
        std::stringstream outputBuffer;
        outputBuffer << file.rdbuf();
        return outputBuffer.str();
    }
    throw runtime_error(std::string(path.c_str()) + " could not be read");
}

void FS::readWholeDirectory(std::filesystem::path p) {
    for(auto const& dir_entry: std::filesystem::recursive_directory_iterator{p}) {
        if(dir_entry.is_regular_file()) {
            std::cout << dir_entry.path() << '\n';
        }
    }
}