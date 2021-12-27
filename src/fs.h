#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <unordered_set>

namespace FS {
    using namespace std;
    string readFile(filesystem::path p);
    std::vector<char> readFileAsBytes(filesystem::path p);
    void readWholeDirectory(filesystem::path p);
}
