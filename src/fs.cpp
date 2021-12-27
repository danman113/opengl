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
    throw runtime_error(path.string() + " could not be read");
}

std::vector<char> FS::readFileAsBytes(filesystem::path path) {
    std::cout << "Reading from " << path << std::endl;
    auto p = std::filesystem::absolute(path);
    std::cout << p << std::endl;
    // Starts a binary iostream at the end
    std::ifstream file{ p, std::ios::binary | std::ios::ate };
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer (size);
    if (file.read(buffer.data(), size)) {
        return buffer;
    }
    throw runtime_error(path.string() + " could not be read");
}

void FS::readWholeDirectory(std::filesystem::path p) {
    for(auto const& dir_entry: std::filesystem::recursive_directory_iterator{p}) {
        if(dir_entry.is_regular_file()) {
            std::cout << dir_entry.path() << '\n';
        }
    }
}