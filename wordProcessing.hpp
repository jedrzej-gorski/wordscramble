#pragma once
#include <array>
#include <fstream>

namespace wordProcessing {
    std::array<char, 16> generateCharset();
    bool validateAnswer(std::array<char, 16>& charset, std::string& answer);
};